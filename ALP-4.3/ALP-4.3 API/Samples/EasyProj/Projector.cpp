//
// Projector.cpp
//

#include "StdAfx.h"
#include "Projector.h"

// error codes of CProjector
const int PROJECTOR_ERROR									= 5001;
const int PROJECTOR_NOT_CONNECTED							= 5002;
const int PROJECTOR_ERROR_SEQ_NOT_VALID						= 5003;
const int PROJECTOR_ERROR_ALREADY_ALLOCATED					= 5010;
const int PROJECTOR_ERROR_SEQ_IMG_SIZE						= 5015;
// text messages for the error codes of CProjector
const wchar_t* MSG_PROJECTOR_ERROR							= L"ERROR";
const wchar_t* MSG_PROJECTOR_NOT_CONNECTED					= L"The projector is not connected.";
const wchar_t* MSG_PROJECTOR_ERROR_SEQ_NOT_VALID			= L"The sequence is not valid.";
const wchar_t* MSG_PROJECTOR_ERROR_ALREADY_ALLOCATED		= L"The object is already allocated.";
const wchar_t* MSG_PROJECTOR_ERROR_SEQ_IMG_SIZE				= L"Sequence image has wrong size.";

// text messages for the error codes of the ALP api
const wchar_t* MSG_OK										= L"Ok";
const wchar_t* NO_ERROR_MSG_AVAILABLE						= L"No Error message available for this error code.";
const wchar_t* ERROR_MSG_GENERAL							= L"ERROR";
const wchar_t* ERROR_MSG_ALP_NOT_ONLINE						= L"The specified ALP has not been found or is not ready.";
const wchar_t* ERROR_MSG_ALP_NOT_IDLE						= L"The ALP is not in idle state.";
const wchar_t* ERROR_MSG_ALP_NOT_AVAILABLE					= L"The specified ALP identifier is not valid.";
const wchar_t* ERROR_MSG_ALP_NOT_READY						= L"The specified ALP is already allocated.";
const wchar_t* ERROR_MSG_ALP_PARM_INVALID					= L"One of the parameters is invalid.";
const wchar_t* ERROR_MSG_ALP_ADDR_INVALID					= L"Error accessing user data.";
const wchar_t* ERROR_MSG_ALP_MEMORY_FULL					= L"The requested memory is not available.";
const wchar_t* ERROR_MSG_ALP_SEQ_IN_USE						= L"The sequence specified is currently in use.";
const wchar_t* ERROR_MSG_ALP_HALTED							= L"The ALP has been stopped while image data transfer was active.";
const wchar_t* ERROR_MSG_ALP_ERROR_INIT						= L"Initialization error.";
const wchar_t* ERROR_MSG_ALP_ERROR_COMM						= L"Communication error.";
const wchar_t* ERROR_MSG_ALP_DEVICE_REMOVED					= L"The specified ALP has been removed.";
const wchar_t* ERROR_MSG_ALP_NOT_CONFIGURED					= L"The onboard FPGA is unconfigured.";
const wchar_t* ERROR_MSG_ALP_LOADER_VERSION					= L"The function is not supported by this version of the driver file VlxUsbLd.sys.";
const wchar_t* ERROR_MSG_ALP_ERROR_API_DLL_MISSING			= L"alpX: The responsible API DLL could not be loaded.";
const wchar_t* ERROR_MSG_ALP_ERROR_POWER_DOWN				= L"Waking up the DMD from PWR_FLOAT did not work (ALP_DMD_POWER_FLOAT).";
const wchar_t* ERROR_MSG_ALP_DRIVER_VERSION					= L"Support in ALP drivers missing. Update drivers and power-cycle device.";
const wchar_t* ERROR_MSG_ALP_SDRAM_INIT						= L"SDRAM Initialization failed.";
const wchar_t* ERROR_MSG_ALP_CONFIG_MISMATCH				= L"The device is not properly configured for a function call with the specified parameters.";



CProjector::CProjector(void)
	: m_DeviceID(ALP_INVALID_ID)
	, m_SequenceID(0)
	, m_ImageIdx(0)
	, m_width(0)
	, m_height(0)
	, m_Led1(m_DeviceID)
	, m_Led2(m_DeviceID)
	, m_Led3(m_DeviceID)
{
}


CProjector::~CProjector(void)
{
}


// get the textmessage for a certain error code
void CProjector::GetErrorMessage(const int errorCode, wchar_t *errorText, const int nChars) const
{
	if(nChars <= 0)												return;
	if(errorText == nullptr )									return;
	if(::IsBadWritePtr(errorText, nChars*sizeof(*errorText)))	return;

	const wchar_t* pErrMsg = ERROR_MSG_GENERAL;
	switch( errorCode)
	{
	case ALP_OK:				pErrMsg = MSG_OK; break;
	case ALP_NOT_ONLINE:		pErrMsg = ERROR_MSG_ALP_NOT_ONLINE; break;
	case ALP_NOT_IDLE:			pErrMsg = ERROR_MSG_ALP_NOT_IDLE; break;
	case ALP_NOT_AVAILABLE:		pErrMsg = ERROR_MSG_ALP_NOT_AVAILABLE; break;
	case ALP_NOT_READY:			pErrMsg = ERROR_MSG_ALP_NOT_READY; break;
	case ALP_PARM_INVALID:		pErrMsg = ERROR_MSG_ALP_PARM_INVALID; break;
	case ALP_ADDR_INVALID:		pErrMsg = ERROR_MSG_ALP_ADDR_INVALID; break;
	case ALP_MEMORY_FULL:		pErrMsg = ERROR_MSG_ALP_MEMORY_FULL; break;
	case ALP_SEQ_IN_USE:		pErrMsg = ERROR_MSG_ALP_SEQ_IN_USE; break;
	case ALP_HALTED:			pErrMsg = ERROR_MSG_ALP_HALTED; break;
	case ALP_ERROR_INIT:		pErrMsg = ERROR_MSG_ALP_ERROR_INIT; break;
	case ALP_ERROR_COMM:		pErrMsg = ERROR_MSG_ALP_ERROR_COMM; break;
	case ALP_DEVICE_REMOVED:	pErrMsg = ERROR_MSG_ALP_DEVICE_REMOVED; break;
	case ALP_NOT_CONFIGURED:	pErrMsg = ERROR_MSG_ALP_NOT_CONFIGURED; break;
	case ALP_LOADER_VERSION:	pErrMsg = ERROR_MSG_ALP_LOADER_VERSION; break;
	case 1016:					pErrMsg = ERROR_MSG_ALP_ERROR_API_DLL_MISSING; break;
	case ALP_ERROR_POWER_DOWN:	pErrMsg = ERROR_MSG_ALP_ERROR_POWER_DOWN; break;
	case ALP_DRIVER_VERSION:	pErrMsg = ERROR_MSG_ALP_DRIVER_VERSION; break;
	case ALP_SDRAM_INIT:		pErrMsg = ERROR_MSG_ALP_SDRAM_INIT; break;
#ifdef ALP_CONFIG_MISMATCH
	case ALP_CONFIG_MISMATCH:	pErrMsg = ERROR_MSG_ALP_CONFIG_MISMATCH; break;
#endif

	case PROJECTOR_ERROR:							pErrMsg = MSG_PROJECTOR_ERROR; break;
	case PROJECTOR_NOT_CONNECTED:					pErrMsg = MSG_PROJECTOR_NOT_CONNECTED; break;
	case PROJECTOR_ERROR_SEQ_NOT_VALID:				pErrMsg = MSG_PROJECTOR_ERROR_SEQ_NOT_VALID; break;
	case PROJECTOR_ERROR_ALREADY_ALLOCATED:			pErrMsg = MSG_PROJECTOR_ERROR_ALREADY_ALLOCATED; break;
	case PROJECTOR_ERROR_SEQ_IMG_SIZE:				pErrMsg = MSG_PROJECTOR_ERROR_SEQ_IMG_SIZE; break;

	default:					pErrMsg = NO_ERROR_MSG_AVAILABLE; break;
	}

	wcscpy_s( errorText, nChars, pErrMsg);
}


// allocate and initialize the projector
int CProjector::Alloc(CString *errorMessage)
{
	if (ALP_INVALID_ID != m_DeviceID)
		return PROJECTOR_ERROR_ALREADY_ALLOCATED;
	
	m_DeviceID = ALP_INVALID_ID;
	m_ImageIdx = 0;
	long lRet = AlpDevAlloc( 0, ALP_DEFAULT, &m_DeviceID);				// call the api function AlpDevAlloc()
	if( ALP_OK != lRet)
	{
		m_DeviceID = ALP_INVALID_ID;
		*errorMessage = L"Error in AlpDevAlloc while allocating the projector.";
	}
	else
	{
		// Get the dimensions of the DMD
		long lVal = 0;

		lRet = AlpDevInquire( m_DeviceID, ALP_DEV_DISPLAY_WIDTH, &lVal);	// call the api function AlpDevInquire

		if( ALP_OK == lRet)
			SetWidth( lVal);
		else if(errorMessage != NULL)
			*errorMessage = L"Error in AlpDevInquire while inquiring the dimensions of the DMD.";

		lRet = AlpDevInquire( m_DeviceID, ALP_DEV_DISPLAY_HEIGHT, &lVal);	// call the api function AlpDevInquire
			
		if( ALP_OK == lRet)	
			SetHeight( lVal);
		else if(errorMessage != NULL)
			*errorMessage = L"Error in AlpDevInquire while inquiring the dimensions of the DMD.";
	}
	return lRet;
}


// free the projector
int CProjector::Free(CString *errorMessage)
{
	if(m_Led1.IsValid())
		m_Led1.Free();
	if(m_Led2.IsValid())
		m_Led2.Free();
	if(m_Led3.IsValid())
		m_Led3.Free();
	
	long lRet = AlpDevFree( m_DeviceID);								// call the api function AlpDevFree()
	
	m_DeviceID = ALP_INVALID_ID;
	m_SequenceID = 0;
	m_ImageIdx = 0;
	m_width = 0;
	m_height = 0;

	if( ALP_OK != lRet && NULL != errorMessage)
		*errorMessage = L"Error in AlpDevFree while releasing the device.";

	return lRet;
}


// true, if the projector was successfully initialized
bool CProjector::IsConnected(void) const
{
	return m_DeviceID != ALP_INVALID_ID;
}


// get the properties of the projector
int CProjector::GetDevProperties(CDevProperties &properties, CString *errorMessage) const
{
	long lRet = ALP_OK;

	// Define a macro GET_DEV_PROP, which calls the api function AlpDevInquire().
#define	GET_DEV_PROP(parm, val)	\
	lRet = AlpDevInquire( m_DeviceID, parm, val); \
	if( ALP_OK != lRet)	\
	{	\
		if(ALP_OK != lRet && NULL != errorMessage) \
		*errorMessage = L"Error in AlpDevInquire while inquiring the properties of the projector."; \
	\
		return lRet; \
	}

	// Use the macro
	GET_DEV_PROP(ALP_DEVICE_NUMBER, &properties.SerialNumber);
	GET_DEV_PROP(ALP_AVAIL_MEMORY, &properties.FreeMemory);
	GET_DEV_PROP(ALP_SYNCH_POLARITY, &properties.Polarity);
	GET_DEV_PROP(ALP_TRIGGER_EDGE, &properties.TriggerInEdge);
#undef	GET_DEV_PROP

	return ALP_OK;
}


// change the synch settings
int CProjector::SetSyncOutputMode(const long mode, CString *errorMessage)
{
	long lRet = AlpDevControl( m_DeviceID, ALP_SYNCH_POLARITY, mode);	// call the api function AlpDevControl()

	if(NULL != errorMessage)
		*errorMessage = L"Error in AlpDevControl while changing the synch settings.";

	return lRet;
}


// allocate and initialize a sequence
int CProjector::SequenceAlloc(const int Bitplanes, const int nPictures, CString *errorMessage)
{
	if( m_SequenceID != 0)
		return PROJECTOR_ERROR_ALREADY_ALLOCATED;

	long lRet = AlpSeqAlloc( m_DeviceID, Bitplanes, nPictures, &m_SequenceID);	// call the api function AlpSeqAlloc()
	if( ALP_OK != lRet)
	{
		if(NULL != errorMessage)
			*errorMessage = L"Error in AlpSeqAlloc while allocating the sequence.";

		m_SequenceID = 0;
	}

	return lRet;
}


// free a sequence
int CProjector::SequenceFree(CString *errorMessage)
{
	long lRet =  AlpSeqFree( m_DeviceID, m_SequenceID);					// call the api function AlpSeqFree()
	if( ALP_OK == lRet)
	{
		m_SequenceID = 0;
		m_ImageIdx = 0;
	}
	else if(NULL != errorMessage)
		*errorMessage = L"Error in AlpSeqFree while releasing the sequence.";

	return lRet;
}


// true, if the sequence was successfully initialized
bool CProjector::IsValidSequence(void) const
{
	return IsConnected() == true && m_SequenceID != 0;
}


// get the properties of the sequence
int CProjector::GetSeqProperties(CTimingEx &timing, CString *errorMessage) const
{
	CTimingEx	t;
	long lRet = ALP_OK;
#define	GET_SEQ_TIMING(parm, val) \
	lRet = AlpSeqInquire( m_DeviceID, m_SequenceID, parm, val);	\
	if( ALP_OK != lRet) \
	{	\
		if(ALP_OK != lRet && NULL != errorMessage) \
		*errorMessage = L"Error in AlpSeqInquire while inquiring the properties of the sequence."; \
	\
		return lRet; \
	}

	GET_SEQ_TIMING(ALP_ILLUMINATE_TIME, &t.IlluminateTime);
	GET_SEQ_TIMING(ALP_PICTURE_TIME, &t.PictureTime);
	GET_SEQ_TIMING(ALP_SYNCH_DELAY, &t.SynchDelay);
	GET_SEQ_TIMING(ALP_SYNCH_PULSEWIDTH, &t.SynchPulseWidth);
#ifdef USE_TRIGGER_DELAY
	GET_SEQ_TIMING(ALP_TRIGGER_IN_DELAY, &t.TriggerInDelay);
#endif
	GET_SEQ_TIMING(ALP_BITNUM, &t.BitNum);
	long BinMode;
	GET_SEQ_TIMING(ALP_BIN_MODE, &BinMode);
#undef GET_SEQ_TIMING

	if (ALP_BIN_UNINTERRUPTED==BinMode)
		t.Uninterrupted = true;
	else
		t.Uninterrupted = false;

	// Take the value even after all calls of the macro.
	timing = t;

	return ALP_OK;
}


// change the properties of the sequence
int CProjector::SetSeqProperties(const CTimingEx &timing, CString *errorMessage)
{
	long lRet = ALP_OK;

	long minIll  = ALP_DEFAULT;
	long minPic  = ALP_DEFAULT;
	long minDark = 0;

	lRet = GetSeqFastestTiming( timing.BitNum, timing.Uninterrupted, minPic, minIll, errorMessage );
	if( ALP_OK != lRet)
		return lRet;
	
	minDark = minPic - minIll;

	long	SynchDelay		= timing.SynchDelay;
	long	SynchPulseWidth	= timing.SynchPulseWidth;
	long	TriggerInDelay = ALP_DEFAULT;
#ifdef USE_TRIGGER_DELAY
	TriggerInDelay = timing.TriggerInDelay;
#endif

	long	IlluminateTime = (timing.IlluminateTime < 1) ? 0 : (timing.IlluminateTime);
	long	PictureTime    = (timing.PictureTime < 1) ? (timing.IlluminateTime + minDark + SynchDelay) : (timing.PictureTime);

	lRet = AlpSeqTiming( m_DeviceID, m_SequenceID,						// call the api function AlpSeqTiming()
			IlluminateTime,
			PictureTime,
			SynchDelay,
			SynchPulseWidth,
			TriggerInDelay);

	if(ALP_OK != lRet && NULL != errorMessage)
		*errorMessage = L"Error in AlpSeqTiming while setting the properties of the sequence.";

	return lRet;
}

// change number of bit planes. Get the possible max timing
int CProjector::GetSeqFastestTiming( const long BitNum, const bool Uninterrupted, long &MinPicTime, long &MinIllTime, CString *errorMessage)
{
	long lRet = ALP_OK;

	lRet = AlpSeqControl( m_DeviceID, m_SequenceID, ALP_BITNUM, BitNum );	// call the api function AlpSeqControl
	if( ALP_OK != lRet)
	{
		if(NULL != errorMessage)
			*errorMessage = L"Error in AlpSeqControl while changing the number of bit planes.";	
		
		return lRet;
	}
	lRet = AlpSeqControl( m_DeviceID, m_SequenceID, ALP_BIN_MODE, Uninterrupted?ALP_BIN_UNINTERRUPTED:ALP_BIN_NORMAL );
	if( ALP_OK != lRet)
	{
		if(NULL != errorMessage)
			*errorMessage = L"Error in AlpSeqControl while changing the number of bit planes.";	
		
		return lRet;
	}

	lRet = AlpSeqInquire( m_DeviceID, m_SequenceID, ALP_MIN_PICTURE_TIME, &MinPicTime);	// call the api function AlpSeqInquire
	if( ALP_OK != lRet)
	{
		if(NULL != errorMessage)
			*errorMessage = L"Error in AlpSeqInquire while inquiring the minimal picture time.";
		
		return lRet;
	}

	lRet = AlpSeqInquire( m_DeviceID, m_SequenceID, ALP_MIN_ILLUMINATE_TIME, &MinIllTime);	// call the api function AlpSeqInquire
	if(ALP_OK != lRet && NULL != errorMessage)
		*errorMessage = L"Error in AlpSeqInquire while inquiring the minimal illuminate time.";
	
	return lRet;
}

// Shortcut to AlpSeqIniquire
	long CProjector::GetSeqProperty( IN const long InquireType )
{
	long Value;
	if (ALP_OK == AlpSeqInquire( m_DeviceID, m_SequenceID, InquireType, &Value))
		return Value;
	else
		return -1;
}

// select maximum possible gray scale (ALP_BITNUM) for a given frame timing
int CProjector::SelectMaxBitnum( IN OUT CTimingEx &timing, CString *errorMessage)
{
	long lRet = ALP_OK;

	BOOL const ForcePictureTime		= ALP_DEFAULT!=timing.PictureTime;
	BOOL const ForceIlluminateTime	= ALP_DEFAULT!=timing.IlluminateTime;

	// Please note: the ALP API automatically adjusts inconsistent parameters.
	// PictureTime has precedence over IlluminateTime.
	// The ALP API description contains detailed notes.

	// Try to realize exactly the requested timing (PictureTime AND IlluminateTime). Find an appropriate Bitnum setting.
	if (ForceIlluminateTime && ForcePictureTime)
	{
		timing.Uninterrupted = false;
		for (timing.BitNum=8; timing.BitNum>=1; timing.BitNum--)
		{
			if (ALP_OK != SetSeqTiming( timing, errorMessage))
				continue;

			if (timing.PictureTime == GetSeqProperty( ALP_PICTURE_TIME )
				&& timing.IlluminateTime == GetSeqProperty( ALP_ILLUMINATE_TIME ))
			{	// Success: ALP API realizes both timing parameters unmodified.
				return ALP_OK;
			}
		}
	}

	// Search a Bitnum setting for that the API accepts (and maybe adjusts) the timing setting.
	timing.Uninterrupted = false;
	for (timing.BitNum=8; timing.BitNum>=2; timing.BitNum--)
	{
		if (ALP_OK != SetSeqTiming( timing, errorMessage))
			continue;

		if (ForcePictureTime && timing.PictureTime == GetSeqProperty( ALP_PICTURE_TIME )
			|| ForceIlluminateTime && timing.IlluminateTime == GetSeqProperty( ALP_ILLUMINATE_TIME ))
		{	// Success: ALP API can realize at least one timing parameter unmodified.
			return ALP_OK;
		}
	}
	// Skip 1-bit ALP_BIN_NORMAL mode, because uninterrupted binary mode is faster (by avoiding dark phase between frames)

	// Try 1-bit uninterrupted display (fastest possible mode)
	timing.BitNum=1;
	timing.Uninterrupted = true;
	// timing.SynchDelay requires some tuning in binary uninterrupted mode.
	// -> high brightness in order to clearly separate colour components over time
	timing.SynchPulseWidth = timing.PictureTime - 2;
	lRet = SetSeqTiming( timing, errorMessage);
	if (ALP_OK != lRet)
		return lRet;

	return ALP_OK;
}

// Set up ALP_BIN_MODE, ALP_BITNUM, and AlpSeqTiming
int CProjector::SetSeqTiming( IN OUT CTimingEx &timing, CString *errorMessage)
{
	long lRet;

	lRet = AlpSeqControl( m_DeviceID, m_SequenceID, ALP_BITNUM, timing.BitNum );
	if (ALP_OK != lRet)
	{
		if(NULL != errorMessage)
			*errorMessage = L"Error in AlpSeqControl while selecting ALP_BITNUM.";
		return lRet;
	}

	lRet = AlpSeqControl( m_DeviceID, m_SequenceID, ALP_BIN_MODE, timing.Uninterrupted?ALP_BIN_UNINTERRUPTED:ALP_BIN_NORMAL );
	if (ALP_OK != lRet)
	{
		if(NULL != errorMessage)
			*errorMessage = L"Error in AlpSeqControl while selecting binary mode.";
		return lRet;
	}

	// Note: In some cases the ALP API automatically adjusts inconsistent parameters.
	lRet = AlpSeqTiming( m_DeviceID, m_SequenceID, timing.IlluminateTime, timing.PictureTime, timing.SynchDelay, timing.SynchPulseWidth,
#ifdef USE_TRIGGER_DELAY
		timing.TriggerInDelay
#else
		ALP_DEFAULT
#endif
		);
	if (ALP_OK!=lRet)
	{
		if (NULL != errorMessage)
			*errorMessage = L"Error in AlpSeqTiming.";
	}

	return lRet;
}

// add an image to the sequence
int CProjector::AddImage(BYTE *pImageData, const int width, const int height, CString *errorMessage)
{
	if( GetWidth() != width	||	GetHeight() != height)
		return PROJECTOR_ERROR_SEQ_IMG_SIZE;

	long lRet = AlpSeqPut( m_DeviceID, m_SequenceID, m_ImageIdx, 1, pImageData);	// call the api function AlpSeqPut()
	if( ALP_OK == lRet)
		m_ImageIdx ++;
	else
		if(NULL != errorMessage)
			*errorMessage = L"Error in AlpSeqPut while adding an image to the sequence.";
	return lRet;
}


// start the continously projection of the sequence
int CProjector::ProjStartContinuous(CString *errorMessage)
{
	long lRet = AlpProjStartCont(m_DeviceID, m_SequenceID);				// call the api function AlpProjStartCont()

	if( ALP_OK == lRet && NULL != errorMessage)
		*errorMessage = L"Error in AlpProjStartCont while starting projection.";

	return lRet;
}


// Is the projection running!
bool CProjector::IsProjection(void) const
{
	if (!IsConnected()) return false;

	long state = 0;
	try
	{
		if( ALP_OK == AlpProjInquire(m_DeviceID, ALP_PROJ_STATE, &state)	// call the api function AlpProjInquire()
		&&	ALP_PROJ_ACTIVE == state)
			return true;
	}
	catch (...)
	{
	}
	return false;
}


// stop the projection
int CProjector::ProjStop(CString *errorMessage)
{
	long lRet = AlpProjHalt(m_DeviceID);								// call the api function AlpProjHalt()
																		// Stop the running projection after the current sequence is finshed.
	if( ALP_OK == lRet)
	{
		lRet = AlpDevHalt(m_DeviceID);									// call the api function AlpDevHalt()
																		// Stop the running projection immediately.
		if(ALP_OK != lRet && NULL != errorMessage)
			*errorMessage = L"Error in AlpDevHalt while stopping projection.";
	}
	else if(NULL != errorMessage)
		*errorMessage = L"Error in AlpProjHalt while stopping projection.";

	return lRet;
}

int CProjector::SetFlip( IN const BOOL bFlip, OUT CString &errorMessage )
{
	long lRet = AlpProjControl( m_DeviceID, ALP_PROJ_UPSIDE_DOWN, bFlip? !ALP_DEFAULT : ALP_DEFAULT );

	if (ALP_OK != lRet)
		errorMessage = L"Error in AlpProjControl(ALP_PROJ_UPSIDE_DOWN";

	return lRet;
}
BOOL CProjector::GetFlip()
{
	long nFlip ( ALP_DEFAULT );
	long lRet;

	lRet = AlpProjInquire( m_DeviceID, ALP_PROJ_UPSIDE_DOWN, &nFlip);
	return nFlip!=ALP_DEFAULT;
}


// Create the LED device
CProjector::CLed &CProjector::Led(int nIndex)
{
	switch (nIndex)
	{
	default: throw;
	case 1: return m_Led1;
	case 2: return m_Led2;
	case 3: return m_Led3;
	}
}


// set the gate for LED control
int CProjector::SetSynchGate( long nGateIndex, tAlpDynSynchOutGate &GateConfig, CString *errorMessage )
{
	long lRet = AlpDevControlEx( m_DeviceID, ALP_DEV_DYN_SYNCH_OUT1_GATE+nGateIndex-1, &GateConfig );	// call the api function AlpDevControlEx()
	
	if(ALP_OK != lRet && NULL != errorMessage)
		*errorMessage = L"Error in AlpDevControl while setting the Synch Gates.";

	return lRet;
}
int CProjector::RetriggerSynchOutWatchdog( long Timeout /* [seconds] */ )
{
#ifdef ALP_DEV_DYN_SYNCH_OUT_WATCHDOG
	return AlpDevControl( m_DeviceID, ALP_DEV_DYN_SYNCH_OUT_WATCHDOG, Timeout );
#else
	return ALP_PARM_INVALID;	// feature not available in the API Header file alp.h
#endif
}
////////////////////////////////////////////////////////////////////////////////

CProjector::CLed::CLed(ALP_ID &AlpDevice)
	: m_AlpDevice(AlpDevice)
	, m_LedId(ALP_INVALID_ID)
	, m_OverTempAlerts(0)
{
}


int CProjector::CLed::Alloc (long AlpLedType, int nI2cIndex, CString *errorMessage)
{
	if (ALP_INVALID_ID!=m_LedId) return PROJECTOR_ERROR_ALREADY_ALLOCATED;

	tAlpHldPt120AllocParams I2cAddr;
	I2cAddr.I2cDacAddr = 0x18+2*nI2cIndex;
	I2cAddr.I2cAdcAddr = 0x40+2*nI2cIndex;
	long lRet = AlpLedAlloc( m_AlpDevice, AlpLedType, &I2cAddr, &m_LedId );	// call the api function AlpLedAlloc()
	if(ALP_OK == lRet)
		m_LedType = AlpLedType;
	else if (NULL != errorMessage)
		*errorMessage = L"Error in AlpLedAlloc while allocating the Led.";

	return lRet;
}


int CProjector::CLed::Free (CString *errorMessage)
{
	long lRet = AlpLedFree( m_AlpDevice, m_LedId );
	m_LedId = ALP_INVALID_ID;
	
	if(ALP_OK != lRet && NULL != errorMessage)
		*errorMessage = L"Error in AlpLedFree while releasing the Led.";	
	
	return lRet;
}


bool CProjector::CLed::IsValid()
{
	return ALP_INVALID_ID != m_LedId;
}


int CProjector::CLed::SetBrightness (int nPercent, CString *errorMessage)
{
	if( 0 > nPercent) nPercent = 0;
	if( 100 < nPercent) nPercent = 100;

	long lRet = AlpLedControl( m_AlpDevice, m_LedId, ALP_LED_BRIGHTNESS, nPercent );	// call the api function AlpLedControl()
	
	if(ALP_OK != lRet &&  NULL != errorMessage)
		*errorMessage = L"Error in AlpLedControl while setting brightness of Led.";
	
	return lRet;
}

int CProjector::CLed::GetMeasuredCurrent(long &lMeasuredCurrent, CString *errorMessage)
{
	long lRet = AlpLedInquire( m_AlpDevice, m_LedId, ALP_LED_MEASURED_CURRENT, &lMeasuredCurrent);	// call the api function AlpLedInquire()

	if(ALP_OK != lRet && NULL != errorMessage)
		*errorMessage = L"Error in AlpLedInquire while inquiring the actual current of the Led.";
	return lRet;
}

int CProjector::CLed::GetThermistorTemperature( long &lTemperature, CString *errorMessage)
{
	long lRet = AlpLedInquire( m_AlpDevice, m_LedId, ALP_LED_TEMPERATURE_REF, &lTemperature );

	if(ALP_OK != lRet && NULL != errorMessage)
		*errorMessage = L"Error in AlpLedInquire while inquiring the actual temperature of the Led.";

	return lRet;
}

int CProjector::CLed::GetJunctionTemperature( OUT long &lTemperature, CString *errorMessage, IN long const OnTime /* micro seconds */, IN long const PeriodTime)
{
	// Junction temperature is critical and must be monitored.
	// But we can only measure temperature of a thermistor, that is mounted at some distance to the junction.

	// The ALP API can calculate an estimation of the junction temperature.
	// This calculation is based on drive current (brightness), LED type, and measured thermistor temperature.
	// However, this calculation assumes continuous-wave operation and does not consider the LED-duty-cycle. (STAR-07 RGB)
	// The computation below is taking the duty-cycle of the LED into account and provides a more accurate approximation.

	// Because the LED is heating up very fast when being turned on,
	// the LED is considered to be turned on permanently when the on-time exceeds MaxOnTime. 

	long MaxOnTime = 3333;
	long lRet = ALP_OK;

	// Calculate corrected estimation of Junction Temperature
	if (OnTime == 0)
	{	// duty cycle 0%
		// --> return *thermistor* temperature without any offset
		return GetThermistorTemperature(lTemperature, errorMessage);

	} else if (OnTime > MaxOnTime || OnTime >= PeriodTime)
	{	// duty cycle 100%, or low frequency switching
		// --> return *junction* temperature without duty cycle correction
		lRet = AlpLedInquire( m_AlpDevice, m_LedId, ALP_LED_TEMPERATURE_JUNCTION, &lTemperature );
		if(ALP_OK != lRet && NULL != errorMessage)
			*errorMessage = L"Error in AlpLedInquire while inquiring the junction temperature of the Led.";
		return lRet;

	} else
	{	// apply duty cycle correction. Inquire *both* temperatures from the ALP LED API.
		long ThermistorTemp = 0, JunctionTemp = 0;

		lRet = GetThermistorTemperature(ThermistorTemp, errorMessage);
		if (ALP_OK != lRet) return lRet;

		lRet = GetJunctionTemperature(JunctionTemp, errorMessage); /* 100% duty cycle, no correction */
		if (ALP_OK != lRet) return lRet;

		long lDiffTj_Tref = JunctionTemp - ThermistorTemp;
		lTemperature = ThermistorTemp + lDiffTj_Tref * OnTime / PeriodTime;
		return ALP_OK;
	}
}

long CProjector::CLed::GetJunctionTempLimit() const
{
	// Junction Temperature Limit is a data sheet value, usually expressed as
	// "Maximum Operating Junction Temperature" or "Absolute Maximum", or Lifetime chart.
	
	long lTempLimit = 0;	// Offset. Here one could enter a common negative value for security margin.

	switch (m_LedType)
	{
	default:
		_ASSERTE( ("GetJunctionTempLimit(): This branch should never be executed. Please review enum CProjector::LedTypeID.\r\n", false) );
		lTempLimit += 80;
		break;
	// Maximum Operating Junction Temperature
	case ALP_HLD_PT120_RED : lTempLimit += 100; break;
	case ALP_HLD_PT120_GREEN : lTempLimit += 140; break;
	case ALP_HLD_PT120TE_BLUE : lTempLimit += 130; break;

	case ALP_HLD_CBT90_UV : lTempLimit += 110; break;
	case ALP_HLD_CBT140_WHITE : lTempLimit += 115; break;
	}

	return lTempLimit;
}
