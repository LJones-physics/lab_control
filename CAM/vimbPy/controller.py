from time import time
from vmbpy import *
from queue import Queue, Empty, Full
from vimbPy.logger import LogHandler
from cv2 import selectROI, destroyWindow
from threading import Thread, Event, RLock

TIMESTAMP_FREQUENCY = 48000000 # 48 MHz

class OutstreamManager:
    _init_lock = RLock()

    def __init__(self):
        self._sinks: dict[str, tuple[Queue, Event]] = {}
        self._lock = RLock()

    def create(self, name: str, maxsize: int = 0, start_enabled: bool = False) -> tuple[Queue, Event]:
        with self._lock:
            if name in self._sinks:
                return self._sinks[name]
            q = Queue(maxsize=maxsize)
            ev = Event()
            if start_enabled:
                ev.set()
            else:
                ev.clear()
            self._sinks[name] = (q, ev)
            return q, ev

    def get(self, name: str) -> tuple[Queue, Event]:
        with self._lock:
            return self._sinks[name]

    def exists(self, name: str) -> bool:
        with self._lock:
            return name in self._sinks

    def delete(self, name: str) -> None:
        with self._lock:
            self._sinks.pop(name, None)

    def list_queues(self) -> list[str]:
        with self._lock:
            return list(self._sinks.keys())
        
    def get_sinks(self) -> list[tuple[Queue, Event]]:
        with self._lock:
            return list(self._sinks.values())
        
    def set_queue_state(self, name:str, state: bool) -> None:
        with self._lock:
            _, ev = self._sinks[name]
            if state:
                ev.is_set()
            else:
                ev.clear()
            return None


class VimbaXController:
    # Make sure only one Vimba instance is running
    vmb_instance = None
    vmb_running = False

    def __init__(
        self, cam_id: str = None, log_dir: str = None, log_level: str = "INFO"
    ):
        """
        VimbaXController class to control a camera using the Vimba API.

        Parameters:
            - cam_id (str): Camera ID.
            - log_dir (str): Directory to save log files.
            - log_level (str): Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL).
            - display_frames (bool): Display frames using OpenCV.
            - save_frames (bool): Save frames to disk.
        """

        # 1. Vimba instances to control the camera
        if VimbaXController.vmb_instance is None:
            VimbaXController.vmb_instance = (
                VmbSystem.get_instance()
            )  # Singleton instance (API interface)
        self.vmb = VimbaXController.vmb_instance

        # 2. Camera status
        self.cam = None  # Camera instance
        self.cam_id = cam_id  # Camera ID

        # 3. Stream dispatcher
        self.ingest_queue = Queue(maxsize=30)
        
        # 4. Logger
        self.log_handler = LogHandler(
            f"vimbPy_{self.cam_id or 'VMB'}",
            log_dir=log_dir,
            log_level=log_level,
        )


    def __enter__(self):
        """
        Start the Vimba instance and connect to the camera.
        """
        # Make sure only one Vimba instance is running
        if self.vmb is not None and not VimbaXController.vmb_running:
            self.vmb._startup()
            VimbaXController.vmb_running = True

        # Display camera names if no camera ID is provided
        if self.cam_id is not None:
            self.cam = self._get_camera_by_id(self.cam_id)

        return self
    

    def __exit__(self, exc_type, exc_value, traceback):
        """
        Stop the recording thread, disconnect from the camera, and shutdown Vimba instance.

        Parameters:
            - exc_type (Exception): Exception type. (No use)
            - exc_value (Exception): Exception value. (No use)
            - traceback (Traceback): Traceback object. (No use)
        """
        # Disconnect from the camera and shutdown Vimba instance
        if self.vmb is not None and VimbaXController.vmb_running:
            self.vmb._shutdown()
            VimbaXController.vmb_running = False


    def list_cameras(self, by: str = "name"):
        """
        Returns a list of available cameras.

        Parameters:
            - by (str): name, id, or serial.
        """
        cams = self.vmb.get_all_cameras()
        self.log_handler.info(
            f"Cameras found: {[getattr(cam, f'get_{by}')() for cam in cams]}"
        )
        return [getattr(cam, f"get_{by}")() for cam in cams]
    

    def _get_camera_by_id(self, id: str):
        """
        Returns a camera instance based on the camera ID.

        Parameters:
            - id (str): Camera ID.
        """
        return self.vmb.get_camera_by_id(id)
    

    def _setup_camera(self):
        """
        Apply camera settings.

        Note: This function is called before starting the acquisition.
        """
        with self.cam as cam:
            try:
                # 1. Try to adjust GeV packet size. This Feature is only available for GigE - Cameras.
                streams = cam.get_streams()
                if not streams:
                    self.log_handler.warning(
                        f"Cam {self.cam.get_id()} - No available streams found for the camera."
                    )
                    return
                stream = streams[0]
                if hasattr(stream, "GVSPAdjustPacketSize"):
                    stream.GVSPAdjustPacketSize.run()
                    while not stream.GVSPAdjustPacketSize.is_done():
                        pass
                    self.log_handler.info(
                        f"Cam {self.cam.get_id()} - Packet size adjusted successfully."
                    )
                else:
                    self.log_handler.info(
                        f"Cam {self.cam.get_id()} - GVSPAdjustPacketSize not supported on this camera."
                    )

                # 2. Reset camera timestamp to 0
                cam.GevTimestampControlReset.run()

            except Exception as e:
                self.log_handler.error(
                    f"Cam {self.cam.get_id()} - Failed to adjust setup settings: {e}"
                )
    
    def frame_handler(self, cam: Camera, stream: Stream, frame: Frame):
        """
        Frame handler for asynchronous image acquisition.
        Used to process the frame, save it to disk, and display it using OpenCV.
        Each frame is processed in a separate thread (one to save and one display).

        Parameters:
            - cam (Camera): Camera instance.
            - stream (Stream): Stream instance.
            - frame (Frame): Frame instance.
        """
        # Pre-process the frame and send it to the outstream queue
        img = frame.convert_pixel_format(PixelFormat.Mono8).as_numpy_ndarray().copy()
        try:
            ts = frame.get_timestamp()
        except Exception:
            ts = time()

        try:
            self.ingest_queue.put_nowait((ts, img))
        except Full:
            # Apply policy: drop oldest, keep newest
            try: self.ingest_queue.get_nowait()
            except Empty: pass
            try: self.ingest_queue.put_nowait((ts, img))
            except Full: pass

        # Return the frame to the camera buffer to continue acquisition
        cam.queue_frame(frame)


    def _dispatcher(self, stop_event: Event, outstream_manager: OutstreamManager):
        while not stop_event.is_set() or not self.ingest_queue.empty():
            try:
                ts_img = self.ingest_queue.get(timeout=0.5)
            except Empty:
                continue

            if self.ingest_queue.qsize() > 0:
                self.log_handler.warning(f"Cam {self.cam.get_id()} - Ingest QUEUE is getting delayed: {self.ingest_queue.qsize()}")

            for q, gate in outstream_manager.get_sinks():
                if gate.is_set(): # queue is active
                    # policy: drop oldest, keep newest
                    try:
                        q.put_nowait(ts_img)
                    except Full:
                        try: q.get_nowait()
                        except Empty: pass
                        try: q.put_nowait(ts_img)
                        except Full: pass

            self.ingest_queue.task_done()


    def record(
        self,
        stop_event: Event,
        ready_event: Event,
        outstream_manager: OutstreamManager
    ):
        """
        Main function to start recording frames from the camera (asynchronous grab).
        Designed as a task to be run in a separate thread.

        Parameters:
            - stop_event (threading.Event): Event to stop the recording task.
            - outstream_manager: Singleton that distributes the cam stream into multiples queues
        """

        # Start dispatcher
        dispatcher_thread = Thread(target=self._dispatcher, args=(stop_event, outstream_manager), daemon=True)
        dispatcher_thread.start()

        # Recording task
        try:
            self._setup_camera()

            with self.cam as cam:
                self.log_handler.info(f"Cam {self.cam.get_id()} - Record started.")

                # Start the acquisition and run the frame handler
                cam.start_streaming(
                    self.frame_handler,
                    buffer_count=10,
                    allocation_mode=AllocationMode.AnnounceFrame,
                )

                # Keep the recording thread running until the stop condition is met
                ready_event.set()
                while not stop_event.is_set():
                    stop_event.wait(1)  # Wait for 1s (must be greater than the camera fps) - save CPU

                # Once the stop event is signalized, stop streaming
                cam.stop_streaming()
                self.log_handler.info(f"Cam {self.cam.get_id()} - Record stopped.")
            
            # Destroy dispatcher thread
            dispatcher_thread.join(timeout=2.0)

        except Exception as e:
            self.log_handler.error(f"Cam {self.cam_id} - Error in start_recording: {e}")


    def single_shot(self):
        """
        Capture a single frame from the camera.
        """
        with self.cam as cam:
            cam.AcquisitionMode.set('SingleFrame')
            cam.TriggerSource.set('Freerun')
            cam.TriggerMode.set('Off')
            frame = cam.get_frame()
            img = frame.convert_pixel_format(PixelFormat.Mono8).as_numpy_ndarray()
            return img


    def set_full_roi(self):
        with self.cam as cam:
            try:
                sensor_width = cam.SensorWidth.get()
                sensor_height = cam.SensorHeight.get()
                cam.OffsetX.set(0)
                cam.OffsetY.set(0)
                cam.Width.set(sensor_width)
                cam.Height.set(sensor_height)

            except Exception as e:
                self.log_handler.error(f"Failed to set ROI: {e}")


    def set_roi(self, select=False, width=None, height=None, region_x=0, region_y=0):
        """
        Configure the Region of Interest (ROI) for the camera.

        Parameters:
            - width (int): Width of the ROI.
            - height (int): Height of the ROI.
            - region_x (int): X offset of the ROI.
            - region_y (int): Y offset of the ROI.
        """
        with self.cam as cam:
            try:
                sensor_width = cam.SensorWidth.get()
                sensor_height = cam.SensorHeight.get()

                width = width if width is not None else sensor_width
                height = height if height is not None else sensor_height

                # Check if the ROI dimensions exceed the sensor limits
                if (region_x + width) > sensor_width or (
                    region_y + height
                ) > sensor_height:
                    raise ValueError("ROI dimensions exceed sensor limits.")

                if select:
                    cam.OffsetX.set(0)
                    cam.OffsetY.set(0)
                    cam.Width.set(sensor_width)
                    cam.Height.set(sensor_height)
                    
                    cam.AcquisitionMode.set('SingleFrame')
                    cam.TriggerSource.set('Freerun')
                    cam.TriggerMode.set('Off')
                    frame = cam.get_frame()
                    img = frame.convert_pixel_format(PixelFormat.Mono8).as_numpy_ndarray()

                    roi = selectROI(
                        f"{self.cam_id} ROI - Press C to cancel selection or Enter to confirm it",
                        img,
                        fromCenter=False,
                        showCrosshair=True,
                    )
                    destroyWindow(
                        f"{self.cam_id} ROI - Press C to cancel selection or Enter to confirm it"
                    )
                    region_x = roi[0] or 0
                    region_y = roi[1] or 0
                    width = roi[2] or sensor_width
                    height = roi[3] or sensor_height

                cam.Width.set(width)
                cam.Height.set(height)
                cam.OffsetX.set(region_x)
                cam.OffsetY.set(region_y)

                self.log_handler.info(
                    f"ROI set to Width={width}, Height={height}, X={region_x}, Y={region_y}"
                )

            except Exception as e:
                self.log_handler.error(f"Failed to set ROI: {e}")


    def get_roi(self):
        """
        Retrieve the current Region of Interest (ROI) settings from the camera.

        Returns:
            dict: ROI parameters (width, height, region_x, region_y).
        """
        with self.cam as cam:
            try:
                roi_info = {
                    "width": cam.Width.get(),
                    "height": cam.Height.get(),
                    "region_x": cam.OffsetX.get(),
                    "region_y": cam.OffsetY.get(),
                    "sensor_width": cam.SensorWidth.get(),
                    "sensor_height": cam.SensorHeight.get(),
                }
                for key, value in roi_info.items():
                    self.log_handler.info(f"Cam {self.cam_id} - ROI {key}: {value}")
                return roi_info
            except Exception as e:
                self.log_handler.error(f"Failed to get ROI: {e}")
                return None
            

if __name__ == "__main__":

    from time import sleep

    out_manager = OutstreamManager()
    q_cam, ev_cam = out_manager.create('cam_out', maxsize=5, start_enabled=True)
    q_exp, ev_exp = out_manager.create('exp_out', maxsize=5, start_enabled=False)

    cam_stop, cam_ready = Event(), Event()
    cam_stop.clear()

    def cam_task():
        with VimbaXController(cam_id="DEV_000F314DE426", log_dir=None, log_level="INFO") as ctrl:  # DEV_000F314CEAE1, DEV_000F314DE426
            
            with ctrl.cam as cam1:
                cam1.TriggerSource.set('Line1')
                cam1.TriggerMode.set('On')
            
            ctrl.record(
                stop_event=cam_stop,
                ready_event=cam_ready,
                outstream_manager=out_manager,
            )

    try:
        cam_thread = Thread(target=cam_task, daemon=True)
        cam_thread.start()
        
        print('Waiting camera start...')
        while not cam_ready.is_set():
            cam_ready.wait(1)

        sleep(3)
        
        buffer_cam = []
        buffer_exp = []

        while not q_exp.empty():
            buffer_exp.append(q_exp.get())

        while not q_cam.empty():
            buffer_cam.append(q_cam.get())


        print(f'Imgs: {len(buffer_cam)}')
        print(f'Imgs exp: {len(buffer_exp)}')

    finally:
        # Stop the measurement
        cam_stop.set()
        cam_thread.join()
