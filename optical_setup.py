import time
import numpy as np

from typing import Dict, Union
from pyALP41 import ALP41Controller
from threading import Thread, Event
from vimbPy import VimbaXController, OutstreamManager


class OpticalSetup:

    def __init__(self):
        self._micromirror_pitch = 10.8 * 1e-6 # in meters
        self.dmd_sn = 16017


    def _on(self):
        self._dmd_on()
        self._cams_on()

    
    def _off(self):
        self._dmd_off()
        self._cams_off()


    def _cams_on(self):
        self.cams: Dict[str, Dict[str, Union[str, Event, OutstreamManager, Thread]]] = {
            'single': {'cam_id': 'DEV_000F314E71C8', 'stop': Event(), 'ready': Event(), 'out_manager': OutstreamManager()},
            'double': {'cam_id': 'DEV_000F314DE426', 'stop': Event(), 'ready': Event(), 'out_manager': OutstreamManager()},
            }
        
        for cam_name, cam in self.cams.items():
            cam['stop'].clear()
            cam['ready'].clear()
            cam['out_manager'].create(f'cam_out', maxsize=1, start_enabled=True)

            self.cams_thread = Thread(target=self.__cam_task, daemon=True)
        
        self.cams_thread.start()

        self.q_single, _ = self.cams['single']['out_manager'].get('cam_out')
        self.q_double, _ = self.cams['double']['out_manager'].get('cam_out')
        
        while not self.cams['single']['ready'].is_set():
            self.cams['single']['ready'].wait(.5)
        while not self.cams['double']['ready'].is_set():
            self.cams['double']['ready'].wait(.5)


    def __cam_task(self):
        with VimbaXController(cam_id=self.cams["single"]["cam_id"], log_dir="./", log_level="INFO") as ctrl_single,\
             VimbaXController(cam_id=self.cams["double"]["cam_id"], log_dir="./", log_level="INFO") as ctrl_double:

            ctrl_controllers = {
                "single": ctrl_single,
                "double": ctrl_double,
            }

            threads = []

            for cam_name, cam_info in self.cams.items():
                ctrl = ctrl_controllers[cam_name]

                def record_wrapper(ctrl_ref, cam_info_ref):
                    with ctrl_ref.cam as cam1:
                        cam1.TriggerSource.set('Line1') # Change (probably)
                        cam1.TriggerMode.set('On')
                    
                    ctrl_ref.record(
                        stop_event=cam_info_ref['stop'],
                        ready_event=cam_info_ref['ready'],
                        outstream_manager=cam_info_ref['out_manager'],
                    )

                t = Thread(target=record_wrapper, args=(ctrl, cam_info), daemon=True)
                t.start()
                cam_info["thread"] = t
                threads.append(t)

            for t in threads:
                t.join()

    
    def _cams_off(self):
        buffer = []
        for c in self.cams.keys():
            q, _ = self.cams[c]['out_manager'].get('cam_out')

            while not q.empty():
                buffer.append(q.get())

            if len(buffer) > 0:
                print(f'Imgs on the queue before turning off: {len(buffer)} - {c}')

        for name, cam in self.cams.items():
            cam['stop'].set()
            cam['thread'].join(timeout=2.0)
        self.cams_thread.join(timeout=2.0)

    
    def _adjust_exposure(self, linear: Union[float, str, None] = None, nonlinear: Union[float, str, None] = None):
        ...


    def _dmd_on(self):
        self.dmd = ALP41Controller(log_dir="./", log_level="INFO").connect(device_sn=self.dmd_sn)
        sbuffer = []
        for _ in range(5):
            sbuffer.append(self.dmd.alloc_memory(bit_planes=1, pic_num=1, bin_mode=True))
        self.sbuffer = np.array(sbuffer).flatten()

    
    def _dmd_off(self):
        self.dmd.idle()
        for s in self.sbuffer:
            self.dmd.free_memory(int(s))
        self.dmd.disconnect()

    
    def _dmd_display(self, pattern: np.ndarray[np.uint8], continuous: bool = False):
        self.seq_id = int(self.sbuffer[0])
        self.dmd.send_imgs_to_mem(seq_id=self.seq_id, img_array=pattern, pic_offset=0)
        self.dmd.display(seq_id=self.seq_id, continuous=continuous)
        self.sbuffer = np.roll(self.sbuffer, 1)