from pyALP41.consts import DLL_PATH, SPECIAL_ALP_DEFAULT, AlpDynSynchOutGate
import ctypes


class ALP41Wrapper:
    def __init__(self):
        self.alp_lib = ctypes.WinDLL(DLL_PATH)

    # API wrapper
    def _allocate_device(
        self,
        device_id: ctypes.POINTER(ctypes.c_uint32),
        device_sn: ctypes.c_int32 = SPECIAL_ALP_DEFAULT,
    ):
        """
        Allocate the device

        Parameters:
        - device_id: Pointer to store the device identifier
        """
        return self.alp_lib.AlpDevAlloc(device_sn, SPECIAL_ALP_DEFAULT, device_id)

    def _free_device(self, device_id: ctypes.c_uint32):
        """
        Free the device

        Parameters:
        - device_id: The device identifier
        """
        return self.alp_lib.AlpDevFree(device_id)

    def _inquire_device(
        self,
        device_id: ctypes.c_int32,
        code: ctypes.c_int32,
        user_var_ptr: ctypes.POINTER(ctypes.c_int32),
    ):
        """
        Inquire a given information from the device

        Parameters:
        - device_id: The device identifier
        - code: The code of the information to inquire
        - user_var_ptr: Pointer to store the information
        """
        return self.alp_lib.AlpDevInquire(device_id, code, user_var_ptr)

    def _halt_device(self, device_id: ctypes.c_int32):
        """
        Halt the device

        Parameters:
        - device_id: The device identifier
        """
        return self.alp_lib.AlpDevHalt(device_id)

    def _control_device(
        self, device_id: ctypes.c_int32, code: ctypes.c_int32, value: ctypes.c_int32
    ):
        """
        Change display properties of the device

        Parameters:
        - device_id: The device identifier
        - code: The code of the property to control
        - value: The value to set
        """
        return self.alp_lib.AlpDevControl(device_id, code, value)

    def _control_device_ex(
        self,
        device_id: ctypes.c_int32,
        code: ctypes.c_int32,
        user_struct_ptr: ctypes.POINTER(AlpDynSynchOutGate),
    ):
        """
        Control the behaviour of the SYNCH_OUTx_GATE signals of the device

        Parameters:
        - device_id: The device identifier
        - code: The output pin code to control
        - user_struct_ptr: Pointer with the configuration structure
        """
        return self.alp_lib.AlpDevControlEx(device_id, code, user_struct_ptr)

    def _alloc_sequence(
        self,
        device_id: ctypes.c_int32,
        bit_planes: ctypes.c_int32,
        pic_num: ctypes.c_int32,
        seq_id: ctypes.POINTER(ctypes.c_uint32),
    ):
        """
        Provides ALP memory for a sequence of pictures.

        Parameters:
        - bit_planes: Bit depth for the sequence (1, 2, 3... 8 bits max)
        - pic_num: Number of pictures in the sequence (depends on the available memory - AVAIL_MEM/bit_planes)
        - seq_id: Pointer to store the identificer of the sequence
        """
        return self.alp_lib.AlpSeqAlloc(device_id, bit_planes, pic_num, seq_id)

    def _free_sequence(self, device_id: ctypes.c_int32, seq_id: ctypes.c_uint32):
        """
        The ALP memory reserved for the specified sequence in the device DeviceId is released.

        Parameters:
        - device_id: The device identifier.
        - seq_id: The sequence identifier of the allocated memory to be released.
        """
        return self.alp_lib.AlpSeqFree(device_id, seq_id)

    def _put_sequence(
        self,
        device_id: ctypes.c_int32,
        seq_id: ctypes.c_uint32,
        pic_offset: ctypes.c_int32,
        pic_load: ctypes.c_int32,
        img_ptr: ctypes.c_void_p,
    ):
        """
        Loads a sequence of pictures into an allocated ALP device memory.

        Parameters:
        - seq_id: The sequence identifier of the allocated memory where the pictures will be loaded.
        - pic_offse: The offset in the sequence where the first picture will be loaded.
        - pic_load: The number of pictures to load.
        - img_ptr: A pointer to the array of image data to be loaded.
        """
        return self.alp_lib.AlpSeqPut(device_id, seq_id, pic_offset, pic_load, img_ptr)

    def _control_sequence(
        self,
        device_id: ctypes.c_int32,
        seq_id: ctypes.c_uint32,
        code: ctypes.c_int32,
        value: ctypes.c_int32,
    ):
        """
        Change the display properties of a sequence

        Parameters:
        - device_id: The device identifier
        - seq_id: The sequence identifier
        - code: The code of the property to control
        - value: The value to set
        """
        return self.alp_lib.AlpSeqControl(device_id, seq_id, code, value)

    def _control_seq_timing(
        self,
        device_id: ctypes.c_int32,
        seq_id: ctypes.c_uint32,
        ALP_ILLUMINATE_TIME: ctypes.c_uint32,
        ALP_PICTURE_TIME: ctypes.c_uint32,
        ALP_SYNCH_DELAY: ctypes.c_uint32,
        ALP_SYNCH_PULSEWIDTH: ctypes.c_uint32,
        ALP_TRIGGER_IN_DELAY: ctypes.c_uint32,
    ):
        """
        Controls the timing properties of the sequence display.

        Parameters:
        - device_id: The device identifier
        - seq_id: The sequence identifier
        - ALP_ILLUMINATE_TIME: The time the picture is illuminated
        - ALP_PICTURE_TIME: The time the picture is displayed
        - ALP_SYNCH_DELAY: The time delay between the trigger and the start of the picture
        - ALP_SYNCH_PULSEWIDTH: The width of the trigger pulse
        - ALP_TRIGGER_IN_DELAY: The delay between the trigger and the start of the picture
        """
        return self.alp_lib.AlpSeqTiming(
            device_id,
            seq_id,
            ALP_ILLUMINATE_TIME,
            ALP_PICTURE_TIME,
            ALP_SYNCH_DELAY,
            ALP_SYNCH_PULSEWIDTH,
            ALP_TRIGGER_IN_DELAY,
        )

    def _inquire_sequence(
        self,
        device_id: ctypes.c_int32,
        seq_id: ctypes.c_uint32,
        code: ctypes.c_int32,
        user_var_ptr: ctypes.POINTER(ctypes.c_int32),
    ):
        """
        Provides information about the settings of the specified picture sequence.

        Parameters:
        - device_id: The device identifier
        - seq_id: The sequence identifier
        - code: The code of the information to inquire
        - user_var_ptr: Pointer to store the information
        """
        return self.alp_lib.AlpSeqInquire(device_id, seq_id, code, user_var_ptr)

    def _start_projection(
        self,
        device_id: ctypes.c_int32,
        seq_id: ctypes.c_uint32,
    ):
        """
        Starts a projection. The sequence is displayed with the number of repetitions
        controlled by ALP_SEQ_REPEAT (once by default). This can be interrupted prematurely
        using the AlpProjHalt function.

        Parameters:
        - device_id: The device identifier
        - seq_id: The sequence identifier of the sequence to be displayed
        """
        return self.alp_lib.AlpProjStart(device_id, seq_id)

    def _start_projection_continuous(
        self,
        device_id: ctypes.c_int32,
        seq_id: ctypes.c_uint32,
    ):
        """
        Displays the specified sequence in an infinite loop.

        Parameters:
        - device_id: The device identifier
        - seq_id: The sequence identifier of the sequence to be displayed
        """
        return self.alp_lib.AlpProjStartCont(device_id, seq_id)

    def _halt_projection(self, device_id: ctypes.c_int32):
        """
        Halts the current projection.

        Parameters:
        - device_id: The device identifier
        """
        return self.alp_lib.AlpProjHalt(device_id)

    def _wait_projection(self, device_id: ctypes.c_int32):
        """
        Waits until the current projection is finished. Using this function during
        the display of an infinite loop (AlpProjStartCont) causes the ALP_PARM_INVALID
        error return value.

        Parameters:
        - device_id: The device identifier
        """

        return self.alp_lib.AlpProjWait(device_id)

    def _inquire_projection(
        self,
        device_id: ctypes.c_int32,
        code: ctypes.c_int32,
        user_var_ptr: ctypes.POINTER(ctypes.c_int32),
    ):
        """
        Provides information about the current projection.

        Parameters:
        - device_id: The device identifier
        - code: The code of the information to inquire
        - user_var_ptr: Pointer to store the information
        """
        return self.alp_lib.AlpProjInquire(device_id, code, user_var_ptr)

    def _control_projection(
        self,
        device_id: ctypes.c_int32,
        code: ctypes.c_int32,
        value: ctypes.c_int32,
    ):
        """
        Controls the system parameters that are in effect for all sequences.
        This function is only allowed if the ALP is in idle wait state (ALP_PROJ_IDLE), which can be
        enforced by the AlpProjHalt function.

        Parameters:
        - device_id: The device identifier
        - code: The code of the property to control
        - value: The value to set
        """
        return self.alp_lib.AlpProjControl(device_id, code, value)
