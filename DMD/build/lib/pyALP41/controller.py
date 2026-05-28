import ctypes
import numpy as np
import pyALP41.consts as consts

from typing import Any, Dict
from pyALP41.logger import LogHandler
from pyALP41.wrapper import ALP41Wrapper


class ProjectionController(ALP41Wrapper):
    """
        Manager of ProjInquire and ProjControl
    """
    _PREFIX = "PROJECTION_ALP_"

    def __init__(self, device_id: int):
        super().__init__()
        self.device_id = device_id

        # Consts mapping
        self._code_map: Dict[str, int] = {}
        for k in dir(consts):
            if k.startswith(self._PREFIX):
                exposed = k[len(self._PREFIX):] # (ex.: 'PROJ_INVERSION')
                self._code_map[exposed] = getattr(consts, k)

        # Validation of parameters
        self._validators: Dict[str, Any] = {
            # "PROJ_INVERSION": lambda v: v in (0, 1),
        }

    # Check API call return values
    def _check_result(self, res: int) -> None:
        if res == consts.RETURN_ALP_OK:
            return
        errors = {
            consts.RETURN_ALP_NOT_AVAILABLE: "(PROJ - NOT_AVAILABLE) The specified ALP identifier is not valid.",
            consts.RETURN_ALP_NOT_READY: "(PROJ - NOT_READY) The specified ALP is in use by another function.",
            consts.RETURN_ALP_PARAM_INVALID: "(PROJ - PARAM_INVALID) One of the parameters is invalid.",
            consts.RETURN_ALP_NOT_IDLE: "(PROJ - NOT_IDLE) the ALP is not in the idle wait state.",
            consts.RETURN_ALP_ADDR_INVALID: "(PROJ - ADDR_INVALID) user data access invalid."
        }
        msg = errors.get(res, f"(PROJ) Error code {res} not documented.")
        raise RuntimeError(msg)

    def _resolve_code(self, name: str) -> int:
        code = self._code_map.get(name)
        if code is None:
            valid = ", ".join(sorted(self._code_map))
            raise AttributeError(f"Projection param unknown: {name}. Valid: {valid}")
        return code

    # Getter and Setter
    def get(self, proj_code: int) -> int:
        value = ctypes.c_long()
        self._check_result(self._inquire_projection(
            device_id=self.device_id,
            code=proj_code,
            user_var_ptr=ctypes.byref(value)
        ))
        return int(value.value)

    def set(self, proj_code: int, proj_value: int) -> None:
        self._check_result(self._control_projection(
            device_id=self.device_id,
            code=proj_code,
            value=int(proj_value)
        ))

    # Sugar: Getter and Setter
    def __setattr__(self, name: str, value: Any) -> None:
        if not name.isupper() or name.startswith("_"):
            return super().__setattr__(name, value)

        code = self._resolve_code(name)
        validator = getattr(self, "_validators", {}).get(name)
        if validator is not None and not validator(value):
            raise ValueError(f"Invalid value for {name}: {value}")
        self.set(code, value)

    def __getattr__(self, name: str) -> Any:
        if name.isupper():
            code = self._resolve_code(name)
            return self.get(code)
        raise AttributeError(name)

    # Help IDE autocomplete
    def __dir__(self):
        return sorted(set(super().__dir__()) | set(self._code_map.keys()))


class SequenceController(ALP41Wrapper):
    """
        Manager of AlpSeqControl, AlpSeqInquire and AlpSeqTiming
    """
    _PREFIX = "SEQUENCE_ALP_"

    def __init__(self, device_id: int, seq_id: int):
        super().__init__()
        self.device_id = device_id
        self.seq_id = seq_id

        # Consts mapping
        self._code_map: Dict[str, int] = {}
        for k in dir(consts):
            if k.startswith(self._PREFIX):
                exposed = k[len(self._PREFIX):] # (ex.: 'SEQ_REPEAT')
                self._code_map[exposed] = getattr(consts, k)

        # Validation of parameters
        self._validators: Dict[str, Any] = {
            # "SEQ_REPEAT": lambda v: v in (0, 1),
        }

    # Check API call return values
    def _check_result(self, res: int) -> None:
        if res == consts.RETURN_ALP_OK:
            return
        errors = {
            consts.RETURN_ALP_NOT_AVAILABLE: "(SEQ - NOT_AVAILABLE) The specified ALP identifier is not valid.",
            consts.RETURN_ALP_NOT_READY: "(SEQ - NOT_READY) The specified ALP is in use by another function.",
            consts.RETURN_ALP_PARAM_INVALID: "(SEQ - PARAM_INVALID) One of the parameters is invalid.",
            consts.RETURN_ALP_SEQ_IN_USE: "(SEQ - SEQ_IN_USE) The specified sequence is currently in use."
        }
        msg = errors.get(res, f"(SEQ) Error code {res} not documented.")
        raise RuntimeError(msg)

    def _resolve_code(self, name: str) -> int:
        code = self._code_map.get(name)
        if code is None:
            valid = ", ".join(sorted(self._code_map))
            raise AttributeError(f"Sequence param unknown: {name}. Valid: {valid}")
        return code

    # Getter and Setter
    def get(self, seq_code: int) -> int:
        value = ctypes.c_long()
        self._check_result(self._inquire_sequence(
            device_id=self.device_id,
            seq_id=self.seq_id,
            code=seq_code,
            user_var_ptr=ctypes.byref(value)
        ))
        return int(value.value)

    def set(self, seq_code: int, seq_value: int) -> None:
        self._check_result(self._control_sequence(
            device_id=self.device_id,
            seq_id=self.seq_id,
            code=seq_code,
            value=int(seq_value)
        ))

    # Sugar: Getter and Setter
    def __setattr__(self, name: str, value: Any) -> None:
        if not name.isupper() or name.startswith("_"):
            return super().__setattr__(name, value)

        code = self._resolve_code(name)
        validator = getattr(self, "_validators", {}).get(name)
        if validator is not None and not validator(value):
            raise ValueError(f"Invalid value for {name}: {value}")
        self.set(code, value)

    def __getattr__(self, name: str) -> Any:
        if name.isupper():
            code = self._resolve_code(name)
            return self.get(code)
        raise AttributeError(name)

    # Help IDE autocomplete
    def __dir__(self):
        return sorted(set(super().__dir__()) | set(self._code_map.keys()))
    
    # Shortcuts
    def adjust_timing(self,
                      ALP_ILLUMINATE_TIME: int = None,
                      ALP_PICTURE_TIME: int = None,
                      ALP_SYNCH_DELAY: int = None,
                      ALP_SYNCH_PULSEWIDTH: int = None,
                      ALP_TRIGGER_IN_DELAY: int = None):
        
        defaults = {
            "ALP_ILLUMINATE_TIME": ALP_ILLUMINATE_TIME,
            "ALP_PICTURE_TIME": ALP_PICTURE_TIME,
            "ALP_SYNCH_DELAY": ALP_SYNCH_DELAY,
            "ALP_SYNCH_PULSEWIDTH": ALP_SYNCH_PULSEWIDTH,
            "ALP_TRIGGER_IN_DELAY": ALP_TRIGGER_IN_DELAY,
        }

        timing_params = {k: (v if v is not None else consts.SPECIAL_ALP_DEFAULT) for k, v in defaults.items()}

        self._check_result(
            self._control_seq_timing(device_id=self.device_id, seq_id=self.seq_id, **timing_params)
        )


class ALP41Controller(ALP41Wrapper):
    def __init__(
        self,
        log_dir: str = None,
        log_level: str = "INFO",
    ):
        super().__init__()
        self.log_handler = (log_dir, log_level) # Logger
        self.device_sn = None # Serial number
        self.device_id = None # Session ID
        self.continuous = False

    def _check_result(self, res):
        if res == consts.RETURN_ALP_OK:
            return

        errors = {
            consts.RETURN_ALP_PARAM_INVALID: "(DEV - PARAM_INVALID) One of the parameters is invalid, or a ControlType is not supported.",
            consts.RETURN_ALP_ADDR_INVALID: "(DEV - ADDR_INVALID) Memory access failed. Check if the pointer contains a valid memory address.",
            consts.RETURN_ALP_NOT_READY: "(DEV - NOT_READY) This return value has different meanings depending on the called function.",
            consts.RETURN_ALP_NOT_IDLE: "(DEV - NOT_IDLE) To execute the function, the ALP must not display any sequence.",
            consts.RETURN_ALP_SEQ_IN_USE: "(DEV - SEQ_IN_USE) Operation can't be performed while the sequence is in use.",
            consts.RETURN_ALP_NOT_AVAILABLE: "(DEV - NOT_AVAILABLE) The specified DeviceId is invalid.",
            consts.RETURN_ALP_ERROR_COMM: "(DEV - ERROR_COMM) A communication error occurred during the operation.",
            consts.RETURN_ALP_DEVICE_REMOVED: "(DEV - REMOVED) The device has been disconnected.",
            consts.RETURN_ALP_LOADER_VERSION: "(DEV - LOADER_VERSION) The DMD has failed to 'wake up' from ALP_DMD_POWER_FLOAT mode.",
            consts.RETURN_ALP_DRIVER_VERSION: "(DEV - DRIVER_VERSION) A feature is missing in the installed ALP drivers. Update drivers and power-cycle device.",
            consts.RETURN_ALP_SDRAM_INIT: "(DEV - SDRAM_INIT) SDRAM Initialization failed. Switch off the device and check on-board SO-DIMM."
        }

        text = errors.get(res, f"(DEV) Error code {res} not documented.")
        raise RuntimeError(text)
            
    def connect(self, device_sn: int):
        self.log_handler = LogHandler(f"ALP41 - SN{device_sn}", log_dir=self.log_handler[0], log_level=self.log_handler[1])
        self.device_sn = device_sn
        cid = ctypes.c_long() 
        self._check_result(
                self._allocate_device(device_sn=self.device_sn, device_id=ctypes.byref(cid))
            )
        self.device_id = int(cid.value)
        self.log_handler.info(f'Device connected. SN: {self.device_sn}, ID: {self.device_id}')

        self.sequences_manager: Dict[int, SequenceController] = {}
        self.projection = ProjectionController(self.device_id)
        self.h, self.w = self.get_dmd_dimensions()

        return self

    def disconnect(self):
        if self.device_sn is not None:
            self._check_result(
                self._free_device(self.device_id)
            )
        self.log_handler.info(f'Device disconnected. SN: {self.device_sn}')
    
    # Device methods
    def activate(self):
        '''
        Activate the DMD for image projection (default).

        Restores power to the micromirrors, allowing sequences to be displayed.
        '''
        self._check_result(
            self._control_device(self.device_id, code=consts.DEVICE_ALP_DEV_DMD_MODE, value=consts.DEVICE_ALP_DMD_RESUME)
        )

    def deactivate(self):
        '''
        Puts the DMD into an inactive "floating" state.

        All micromirrors are released from their deflected positions, entering a near-flat state.
        Sequence display is disabled, but the FPGA remains programmable.
        '''
        self._check_result(
            self._control_device( self.device_id, code=consts.DEVICE_ALP_DEV_DMD_MODE, value=consts.DEVICE_ALP_DMD_POWER_FLOAT)
        )

    def idle(self):
        '''
        Immediately halts all device operations (emergency stop).

        Puts the device into an idle state where no sequences are executed, and all ongoing processes are stopped.
        This is typically used for emergency stops or safe shutdown procedures.
        '''
        self._check_result(
            self._halt_device(device_id=self.device_id)
        )

    def configure_trigger_out(
        self,
        trigger_out_pin: ctypes.c_long,
        period: ctypes.c_ubyte,
        polarity: ctypes.c_ubyte,
        pattern: list[bool],
    ):
        """
        Configure the trigger out signal of the device.

        Args:
            device_id: The device identifier
            code: The code of the property to control
            period: Period in which gate pattern is repeated
            polarity: Defines the polarity of the gate signal (0 = low, 1 = high)
            pattern: List of boolean values to define the pattern
        """
        config = consts.AlpDynSynchOutGate()
        config.Period = period
        config.Polarity = polarity
        for i, v in enumerate(pattern):
            config.Gate[i] = v
        return self._check_result(self._control_device_ex(self.device_id, trigger_out_pin, config))
    
    def alloc_memory(
        self,
        bit_planes: int,
        pic_num: int,
        bin_mode: bool = True
    ):
        '''
        Allocates memory on the DMD for storing a sequence of images.

        Optionally allows setting sequence and timing parameters at the time of allocation.

        Args:
            bit_planes (int): Number of bit planes per image.
            pic_num (int): Number of images in the sequence.
            bin_mode (bool): True binary mode (no dark phase). Consult the API for more details.

        Returns:
            int: The ID of the allocated sequence (seq_id).
        '''
        # Default initialization
        seq_id = ctypes.c_long()
        self._check_result(
            self._alloc_sequence(self.device_id, bit_planes, pic_num, ctypes.byref(seq_id))
        )
        self.log_handler.info(f"Memory allocated successfully. SeqID: {seq_id.value}")
        
        if seq_id.value not in self.sequences_manager.keys():
            self.sequences_manager[seq_id.value] = SequenceController(self.device_id, seq_id.value)
        else:
            raise ValueError('Sequence already exists.')

        if bin_mode:
            self.sequences_manager[seq_id.value].BIN_MODE = consts.SEQUENCE_ALP_BIN_UNINTERRUPTED
            self.sequences_manager[seq_id.value].adjust_timing()
        
        return seq_id.value
        
    def sequence(self, seq_id: int):
        if seq_id not in self.sequences_manager.keys():
            raise KeyError("Sequence ID not found.")
        return self.sequences_manager[seq_id]

    def free_memory(self, seq_id: int):
        '''
        Frees the memory associated with a previously allocated image sequence.

        Args:
            seq_id (int): The ID of the sequence to be freed.
        '''
        self._check_result(self._free_sequence(self.device_id, seq_id))
        self.sequences_manager.pop(seq_id)
        self.log_handler.info(f"Memory freed successfully. SeqID: {seq_id}")

    def get_curr_avail_memory(self):
        '''
        Retrieves the currently available memory on the DMD.

        This can be used to check whether there is enough space to allocate a new image sequence.
        '''
        value = ctypes.c_long()
        self._check_result(
            self._inquire_device(self.device_id, consts.DEVICE_ALP_AVAIL_MEMORY, ctypes.byref(value))
        )
        self.log_handler.info(f'Available memory: {value.value}')
        return value.value
    
    def get_dmd_dimensions(self):
        '''
        Retrieves the DMD height and width in pixels.
        '''
        h = ctypes.c_long()
        w = ctypes.c_long()
        
        self._check_result(
            self._inquire_device(self.device_id, consts.DEVICE_ALP_DEV_DISPLAY_HEIGHT, ctypes.byref(h))
        )
        self._check_result(
            self._inquire_device(self.device_id, consts.DEVICE_ALP_DEV_DISPLAY_WIDTH, ctypes.byref(w))
        )
        self.h = h.value
        self.w = w.value

        return self.h, self.w

    def send_imgs_to_mem(
        self,
        seq_id: int,
        img_array: np.ndarray,  # (N,1,H,W), uint8, {0,255}
        pic_offset: int = 0
    ) -> None:
        '''Send 1-bit images to the device's RAM memory.'''

        if not hasattr(self, "sequences_manager") or seq_id not in self.sequences_manager:
            raise KeyError(f"Sequence ID not found: {seq_id}")

        if not isinstance(img_array, np.ndarray):
            raise TypeError("img_array must be a numpy.ndarray.")

        if img_array.dtype != np.uint8:
            raise TypeError("Images must be uint8.")

        if img_array.ndim != 4:
            raise ValueError("Expected shape (N,1,H,W).")

        n, c, h, w = img_array.shape
        if c != 1:
            raise ValueError("Expected shape (N,1,H,W) for 1-bit sequences.")
        if h != self.h or w != self.w:
            raise ValueError(f"Image shape must match DMD (H={self.h}, W={self.w}).")

        uniq = np.unique(img_array)
        if not set(uniq.tolist()).issubset({0, 255}):
            raise ValueError("Binary images only (values must be 0 or 255).")

        if pic_offset < 0:
            raise ValueError("pic_offset must be >= 0.")

        # Make the image contiguous and cast into a pointer
        img_array = np.ascontiguousarray(img_array, dtype=np.uint8)
        img_ptr = ctypes.c_void_p(int(img_array.ctypes.data))

        # Send to device's RAM
        self._check_result(self._put_sequence(
            self.device_id,
            seq_id,
            pic_offset=int(pic_offset),
            pic_load=int(n),
            img_ptr=img_ptr,
        ))

    def display(
        self,
        seq_id: int,
        continuous: bool = False
    ) -> None:
        '''Display a sequence already loaded in device's RAM memory.'''

        if not hasattr(self, "sequences_manager") or seq_id not in self.sequences_manager:
            raise KeyError(f"Sequence ID not found: {seq_id}")

        try:
            if continuous:
                self._check_result(self._start_projection_continuous(self.device_id, seq_id))
            else:
                self._check_result(self._start_projection(self.device_id, seq_id))
                self._check_result(self._wait_projection(self.device_id))
        except Exception:
            raise

    def stop_display(self):
        self._check_result(self._halt_projection(self.device_id))
        self._check_result(self._wait_projection(self.device_id))
        self.continuous = False


if __name__ == "__main__":

    # Some patterns to display
    full_white = (np.ones(shape=(1, 1, 1080, 1920)) * 255).astype(np.uint8)
    full_black = (np.zeros(shape=(1, 1, 1080, 1920)) * 0).astype(np.uint8)

    n_squares = 30
    rows = (np.arange(consts.DMD_HEIGHT) // (consts.DMD_HEIGHT // n_squares)).reshape(-1, 1) # shape (H,1)
    cols = (np.arange(consts.DMD_WIDTH) // (consts.DMD_WIDTH // n_squares)).reshape(1, -1) # shape (1,W)
    pattern = (rows + cols) % 2
    chess_pattern = (pattern[np.newaxis, np.newaxis, :, :] * 255).astype(np.uint8) # shape (1,1,H,W)

    border_pattern = np.zeros((consts.DMD_HEIGHT-100, consts.DMD_WIDTH-100), dtype=np.uint8)
    border_pattern = np.pad(border_pattern, ((50, 50), (50, 50)), constant_values=255)
    border_pattern = border_pattern[np.newaxis, np.newaxis, :, :]

    radius = 100
    Y, X = np.ogrid[:consts.DMD_HEIGHT, :consts.DMD_WIDTH]
    center_y = consts.DMD_HEIGHT // 2
    center_x = consts.DMD_WIDTH // 2
    mask = (X - center_x)**2 + (Y - center_y)**2 <= radius**2
    circle_pattern = np.zeros((consts.DMD_HEIGHT, consts.DMD_WIDTH), dtype=np.uint8)
    circle_pattern[mask] = 255
    circle_pattern = circle_pattern[np.newaxis, np.newaxis, :, :]

    # Testing the controller
    dmd = ALP41Controller(log_level='INFO').connect(device_sn=10071)

    try:
        seq_circle = dmd.alloc_memory(bit_planes=1, pic_num=1, bin_mode=True)
        seq_black = dmd.alloc_memory(bit_planes=1, pic_num=1, bin_mode=True)
        seq_white = dmd.alloc_memory(bit_planes=1, pic_num=1, bin_mode=True)
        seq_chess = dmd.alloc_memory(bit_planes=1, pic_num=1, bin_mode=True)
        seq_border = dmd.alloc_memory(bit_planes=1, pic_num=1, bin_mode=True)

        dmd.send_imgs_to_mem(seq_id=seq_circle, img_array=circle_pattern, pic_offset=0)
        dmd.send_imgs_to_mem(seq_id=seq_black, img_array=full_black, pic_offset=0)
        dmd.send_imgs_to_mem(seq_id=seq_white, img_array=full_white, pic_offset=0)
        dmd.send_imgs_to_mem(seq_id=seq_chess, img_array=chess_pattern, pic_offset=0)
        dmd.send_imgs_to_mem(seq_id=seq_border, img_array=border_pattern, pic_offset=0)

        dmd.projection.PROJ_INVERSION = 0

        display_options = [['CIRCLE', seq_circle], ['BLACK', seq_black], ['WHITE', seq_white], ['CHESS', seq_chess], ['BORDER', seq_border]]

        while True:
            for seq_id in display_options:
                if seq_id[0] == 'STANDBY':
                    dmd.stop_display()
                    dmd.deactivate()
                    input(f'Displaying {seq_id[0]}')
                    dmd.activate()
                else:
                    dmd.display(seq_id=seq_id[1], continuous=True)
                    input(f'Displaying {seq_id[0]}')

    except KeyboardInterrupt:
        dmd.idle()

        dmd.free_memory(seq_id=seq_circle)
        dmd.free_memory(seq_id=seq_black)
        dmd.free_memory(seq_id=seq_white)
        dmd.free_memory(seq_id=seq_chess)
        dmd.free_memory(seq_id=seq_border)

        dmd.disconnect()
        print('End of execution.')
