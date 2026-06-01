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
            'single': {'cam_id': 'DEV_000F314CD29C', 'stop': Event(), 'ready': Event(), 'out_manager': OutstreamManager()}}
        
        for cam_name, cam in self.cams.items():
            cam['stop'].clear()
            cam['ready'].clear()
            cam['out_manager'].create(f'cam_out', maxsize=1, start_enabled=True)

            cam['thread'] = Thread(target=self.__cam_task, daemon=True)
        
        self.cams['single']['thread'].start()

        self.q_single, _ = self.cams['single']['out_manager'].get('cam_out')

        
        while not self.cams['single']['ready'].is_set():
            self.cams['single']['ready'].wait(.5)
   


    def __cam_task(self):
        with VimbaXController(
            cam_id=self.cams["single"]["cam_id"],
            log_dir="./",
            log_level="INFO"
        ) as ctrl_single:

            def record_wrapper():

                with ctrl_single.cam as cam1:

                    self.cam_single = cam1

                    cam1.TriggerSelector.set('FrameStart')
                    cam1.TriggerSource.set('Software')
                    cam1.TriggerMode.set('On')

                    ctrl_single.record(
                        stop_event=self.cams['single']['stop'],
                        ready_event=self.cams['single']['ready'],
                        outstream_manager=self.cams['single']['out_manager'],
                    )

            t = Thread(target=record_wrapper, daemon=True)
            t.start()
            t.join()

    def trigger_camera(self):
        self.cam_single.TriggerSoftware.run()
    
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