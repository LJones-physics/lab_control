#include "AlpSimpleApi.h"
#include <sstream>
using namespace std;

CAlpSimpleApi::CAlpSimpleApi(void) :
	mDevId(ALP_INVALID_ID),
	mDmdWidth(1024), mDmdHeight(768),
	mAoiTop(0), mAoiRowCount(0),
	mSeqBitplanes(0),
	mSeqId(ALP_INVALID_ID)
{
}

CAlpSimpleApi::~CAlpSimpleApi(void)
{
	devClose();
}

void CAlpSimpleApi::devOpen(long serialNumber, long flags)
{
	AlpError( AlpDevAlloc(serialNumber, flags, &mDevId), "AlpDevAlloc" );
	AlpError( AlpDevInquire(mDevId, ALP_DEV_DISPLAY_WIDTH, &mDmdWidth), "Read DMD width" );
	AlpError( AlpDevInquire(mDevId, ALP_DEV_DISPLAY_HEIGHT, &mDmdHeight), "Read DMD height" );
}

void CAlpSimpleApi::devClose()
{
	if (ALP_INVALID_ID != mDevId)
	{
		AlpError( AlpDevHalt(mDevId), "Stop ALP operation" );
		AlpError( AlpDevFree(mDevId), "Release ALP handle" );
		mDevId = ALP_INVALID_ID;
		mSeqBitplanes = 0;
		mSeqId = ALP_INVALID_ID;
		mDmdWidth = mDmdHeight = 0;
		mAoiTop = mAoiRowCount = 0;
	}
}

std::string CAlpSimpleApi::alpCallString( LPCSTR functionName, long type, long value )
{
	stringstream operation;
	operation << functionName << "(" << type << ", " << value << ")";
	return operation.str();
}
std::string CAlpSimpleApi::alpCallString( LPCSTR functionName, long type )
{
	stringstream operation;
	operation << functionName << "(" << type << ")";
	return operation.str();
}
void CAlpSimpleApi::devControl( long type, long value ) throw (...)
{
	AlpError( AlpDevControl(mDevId, type, value ), alpCallString( "AlpDevControl", type, value ) );
}
void CAlpSimpleApi::devInquire( long type, long &result ) throw (...)
{
	AlpError( AlpDevInquire(mDevId, type, &result ), alpCallString( "AlpDevInquire", type ) );
}
long CAlpSimpleApi::devInquire( long type ) throw (...)
{
	long result(-1);
	AlpError( AlpDevInquire(mDevId, type, &result ), alpCallString( "AlpDevInquire", type ) );
	return result;
}

void CAlpSimpleApi::seqAlloc(long const nBitplanes, long const nPictures) throw (...)
{
	if (mSeqId != ALP_INVALID_ID)
		AlpError(AlpSeqFree(mDevId, mSeqId), "delete old ALP sequence");
	mSeqId = ALP_INVALID_ID;
	AlpError(AlpSeqAlloc(mDevId, nBitplanes, nPictures, &mSeqId), "allocate new ALP sequence");
	mSeqBitplanes = nBitplanes;
}

void CAlpSimpleApi::seqPut(unsigned char * const data, long picOffset, long picLoad) throw (...)
{
	AlpError(AlpSeqPut(mDevId, mSeqId, picOffset, picLoad, data), "download image data");
}

void CAlpSimpleApi::setSeqScroll( long stepRows, long firstTopRow, long lastTopRow ) throw (...)
{
	AlpError(AlpSeqControl(mDevId, mSeqId, ALP_LINE_INC, stepRows), "set ALP_LINE_INC");
	AlpError(AlpSeqControl(mDevId, mSeqId, ALP_SCROLL_FROM_ROW, firstTopRow), "set ALP_SCROLL_FROM_ROW");
	AlpError(AlpSeqControl(mDevId, mSeqId, ALP_SCROLL_TO_ROW, lastTopRow), "set ALP_SCROLL_TO_ROW");
}
void CAlpSimpleApi::setSeqNoScroll() throw (...)
{
	AlpError(AlpSeqControl(mDevId, mSeqId, ALP_LINE_INC, ALP_DEFAULT), "restore ALP_LINE_INC");
	AlpError(AlpSeqControl(mDevId, mSeqId, ALP_SCROLL_FROM_ROW, 0), "ALP_SCROLL_FROM_ROW=0");
	AlpError(AlpSeqControl(mDevId, mSeqId, ALP_SCROLL_TO_ROW, (seqInquire(ALP_PICNUM)-1)*mDmdHeight), "ALP_SCROLL_TO_ROW=maximum");
}
void CAlpSimpleApi::seqControl( long type, long value ) throw (...)
{
	AlpError( AlpSeqControl(mDevId, mSeqId, type, value ), alpCallString( "AlpSeqControl", type, value ) );
}
void CAlpSimpleApi::seqInquire( long type, long &result ) throw (...)
{
	AlpError( AlpSeqInquire(mDevId, mSeqId, type, &result ), alpCallString( "AlpSeqInquire", type ) );
}
long CAlpSimpleApi::seqInquire( long type ) throw (...)
{
	long result(-1);
	AlpError( AlpSeqInquire(mDevId, mSeqId, type, &result ), alpCallString( "AlpSeqInquire", type ) );
	return result;
}
void CAlpSimpleApi::seqTiming( long pictureTime ) throw (...)
{
	AlpError( AlpSeqTiming(mDevId, mSeqId, 0, pictureTime, 0, 0, 0), "AlpSeqTiming (only PictureTime)" );
}

void CAlpSimpleApi::projStart(long nRepetitionCount) throw (...)
{
	AlpError(AlpSeqControl(mDevId, mSeqId, ALP_SEQ_REPEAT, nRepetitionCount), "Adjust repetition count");
	AlpError(AlpProjStart(mDevId, mSeqId), "Start display");
}

void CAlpSimpleApi::projWait() throw (...)
{
	AlpError(AlpProjWait(mDevId), "AlpProjWait");
}

void CAlpSimpleApi::projStartCont() throw (...)
{
	AlpError(AlpProjStartCont(mDevId, mSeqId), "Start continuous display");
}

void CAlpSimpleApi::projStop() throw (...)
{
	AlpError(AlpProjHalt(mDevId), "AlpProjHalt");
	AlpError(AlpProjWait(mDevId), "AlpProjWait");
}

void CAlpSimpleApi::setBinaryAOI(long topRow, long rowCount)
{
	AlpError( AlpDevControl( mDevId, ALP_SEQ_DMD_LINES, MAKELONG(topRow, rowCount)), "prepare ALP device for AOI operation" );
	mAoiTop = topRow;
	mAoiRowCount = rowCount;
}

void CAlpSimpleApi::setBplut(const CAlpBplut &bplut)
{
	AlpError( AlpProjControlEx( mDevId, ALP_BPLUT_WRITE, (void*) bplut.getAlpBplut()), "download BPLUT to ALP device" );
}
