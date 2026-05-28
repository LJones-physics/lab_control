// ALP Area of Interest Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "alp.h"
#include <memory>
#include <windows.h> 

// Error handling policy: Quit, whenever an ALP error happens. ////////////////
#define VERIFY_ALP(AlpApiCall) { long Ret=AlpApiCall; if (ALP_OK!=Ret) { _tprintf( _T("ALP API Error Code %i (see alp.h) in function\r\n %s\r\nPress any key to exit."), Ret, _T(#AlpApiCall) ); _gettch(); return 1;} }

#define PRESS_KEY(strMessage) {	_tprintf( strMessage ); while (_kbhit()) _gettch(); _gettch();}


int _tmain(int /* argc */, _TCHAR* /* argv */[])
{
	_tprintf( _T("This ALP Area of Interest Sample demonstrates \r\nhow to correctly combine Area of Interest with \r\ndata download via AlpSeqPutEx. \r\n------------------------------------------------------------------\r\n\r\n"));



// General declarations and definitions ///////////////////////////////////////
	ALP_ID AlpDevId		= 0;
	ALP_ID AlpSeqId		= 0;
	long nRows			= 0;
	long nColumns		= 0;
	long nFrames		= 3;
	long nSeqFrames		= 0;
	long nAoiFirstRow	= 0;
	long nAoiRowCount	= 0;
	long nMinPicTime	= 0;
	long nPicTime		= 1000000;

	tAlpLinePut AlpLinePut;

// Initialize ALP /////////////////////////////////////////////////////////////
	VERIFY_ALP(AlpDevAlloc(0, 0, &AlpDevId));

	VERIFY_ALP(AlpDevInquire(AlpDevId, ALP_DEV_DISPLAY_HEIGHT, &nRows));
	VERIFY_ALP(AlpDevInquire(AlpDevId, ALP_DEV_DISPLAY_WIDTH, &nColumns));

	nAoiFirstRow = nRows/4;
	nAoiRowCount = nRows/2;

	VERIFY_ALP(AlpDevControl(AlpDevId, ALP_SEQ_DMD_LINES, MAKELONG(nAoiFirstRow, nAoiRowCount)));

// Set up Sequence ////////////////////////////////////////////////////////////
	std::unique_ptr<char unsigned[]> pSeqData(new char unsigned[nFrames*nAoiRowCount*nColumns]);
	long nRectangleFirstColumn	= 0;
	long nRectangleLastColumn	= 0;
	long nRectangleFirstRow		= nAoiRowCount/4;
	long nRectangleLastRow		= nAoiRowCount*3/4;


	for(long FrameCount = 0; FrameCount < nFrames; FrameCount++)
	{
		switch(FrameCount)
		{
		case 0:
			nRectangleFirstColumn = nColumns/4; 
			nRectangleLastColumn  = nRectangleFirstColumn + nColumns/3;
			break;
		case 1:
			nRectangleFirstColumn = nColumns/3; 
			nRectangleLastColumn  = nRectangleFirstColumn + nColumns/3;		
			break;
		case 2:
			nRectangleFirstColumn = nColumns*5/12; 
			nRectangleLastColumn  = nRectangleFirstColumn + nColumns/3;
			break;
		}

		for(long y=0; y < nAoiRowCount; y++)
		{
			for(long x=0; x < nColumns; x++)
			{
				if( x >= nRectangleFirstColumn && x < nRectangleLastColumn && y >= nRectangleFirstRow && y < nRectangleLastRow )
					pSeqData[FrameCount*nAoiRowCount*nColumns + y*nColumns + x] = 0x00;
				else
					pSeqData[FrameCount*nAoiRowCount*nColumns + y*nColumns + x] = 0xFF;
			}
		}
	}

	// Compute the number of necessary sequence-frames
	nSeqFrames = (nFrames*nAoiRowCount + nRows - 1) / nRows;

	// Allocate Sequence
	VERIFY_ALP(AlpSeqAlloc(AlpDevId, 1, nSeqFrames, &AlpSeqId));

	// Set up the AOI for the Sequence
	VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId, ALP_SEQ_DMD_LINES, MAKELONG(nAoiFirstRow, nAoiRowCount)));

	// Inquire minimum PictureTime
	VERIFY_ALP(AlpSeqInquire(AlpDevId, AlpSeqId, ALP_MIN_PICTURE_TIME, &nMinPicTime));
	_tprintf( _T("Maximum Frame rate of the sequence: %i Hz \r\n"), 1000000 / nMinPicTime);

	// Activate AOI for the Sequence
	VERIFY_ALP(AlpSeqTiming(AlpDevId, AlpSeqId, ALP_DEFAULT, nPicTime, ALP_DEFAULT, ALP_DEFAULT, ALP_DEFAULT ));
	_tprintf( _T("Current Frame rate of the sequence: %i Hz \r\n\r\n"), 1000000 / nPicTime);


	AlpLinePut.TransferMode	= ALP_PUT_LINES;
	AlpLinePut.LineOffset	= ALP_DEFAULT;
	AlpLinePut.LineLoad		= ALP_DEFAULT;
	AlpLinePut.PicOffset	= 0;
	AlpLinePut.PicLoad		= nFrames;

	VERIFY_ALP(AlpSeqPutEx(AlpDevId, AlpSeqId, &AlpLinePut, pSeqData.get()));
	VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId, ALP_LINE_INC, nAoiRowCount));


// Start projection ///////////////////////////////////////////////////////////
	VERIFY_ALP(AlpProjStartCont(AlpDevId, AlpSeqId));
	
	PRESS_KEY(_T("Starting sequence-display. Press any key to exit.\r\n\r\n"));

// Deallocate device //////////////////////////////////////////////////////////
	VERIFY_ALP(AlpDevHalt(AlpDevId));
	VERIFY_ALP(AlpDevFree(AlpDevId));

	return 0;
}