#pragma once

#include "alp.h"
#include "AlpException.h"
#include "AlpBplut.h"
#include <Windows.h>

/** Object oriented interface to ViALUX ALP API functions.
	Simple sequence handling: manage only one device and one sequence per instance
	Error handling: throw CAlpException
*/
class CAlpSimpleApi
{
public:
	CAlpSimpleApi(void);
	~CAlpSimpleApi(void);
	
	/// Allocate device, inquire some information
	void devOpen(long serialNumber=0, long flags=ALP_DEFAULT) throw (...);

	/// Halt and free device
	void devClose() throw (...);

	/// ALP Device setup and inquiry:
	/// call AlpDevControl
	void devControl( long type, long value ) throw (...);
	/// call AlpDevInquire
	void devInquire( long type, long &result ) throw (...);
	/// call AlpDevInquire
	long devInquire( long type ) throw (...);
	/// area of interest: active DMD rows for cropped display of binary sequences. Call AlpDevControl(ALP_SEQ_DMD_LINES).
	void setBinaryAOI(long topRow, long rowCount) throw (...);
	/// getter for DMD area of interest. packed/encoded numbers suitable for AlpSeqControl(ALP_SEQ_DMD_LINES)
	long getBinaryAOI() const
	{ return MAKELONG(mAoiTop, mAoiRowCount ); }
	/// getter for size of DMD area of interest
	long getBinaryAoiHeight() const
	{ return mAoiRowCount; }
	
	/// getter for DMD Format
	long getDmdWidth() const
	{ return mDmdWidth; }
	/// getter for DMD Format
	long getDmdHeight() const
	{ return mDmdHeight; }

	/// BPLUT
	void setBplut(const CAlpBplut &bplut);

	/// Sequence setup:
	/// AlpSeqAlloc
	void seqAlloc(long const bitplanes, long const pictures) throw (...);
	/// getter function
	long getSeqBitplanes() const
	{ return mSeqBitplanes; }
	/// AlpSeqPut
	void seqPut(unsigned char * const data, long picOffset=ALP_DEFAULT, long picLoad=ALP_DEFAULT) throw (...);
	/// AlpSeqControl: Scrolling mode
	void setSeqScroll( long stepRows, long firstTopRow, long lastTopRow ) throw (...);
	/// AlpSeqControl: restore default behaviour, not scrolling
	void setSeqNoScroll() throw (...);
	/// AlpSeqControl
	void seqControl( long type, long value ) throw (...);
	/// AlpSeqInquire
	void seqInquire( long type, long &result ) throw (...);
	/// AlpSeqInquire
	long seqInquire( long type ) throw (...);
	/// AlpSeqTiming
	void seqTiming( long pictureTime ) throw (...);
	
	/// Sequence display:
	/// set ALP_SEQ_REPEAT and AlpProjStart
	void projStart(long nRepetitionCount) throw (...);
	/// AlpProjWait
	void projWait() throw (...);
	/// AlpProjStartCont
	void projStartCont() throw (...);
	/// synchronous and blocking stop:
	/// AlpProjHalt (run until end of current iteration) and AlpProjWait
	void projStop() throw (...);

private:
	/// generate the "operation" string to be displayed in case of an error
	static std::string alpCallString( LPCSTR functionName, long type, long value );
	/// generate the "operation" string to be displayed in case of an error
	static std::string alpCallString( LPCSTR functionName, long type );

	/// ALP device ID (CAlpSimpleApi supports one device per instance)
	ALP_ID mDevId;
	/// DMD size
	long mDmdWidth, mDmdHeight;

	/// pre-configured AOI size
	long mAoiTop, mAoiRowCount;

	/// Cache this parameter. Same as seqInquire(ALP_BITPLANES).
	long mSeqBitplanes;

	/// ALP sequence ID (CAlpSimpleApi supports only one sequence per instance)
	ALP_ID mSeqId;
};

