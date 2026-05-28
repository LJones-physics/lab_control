// ALP DMD-Mask Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "alp.h"
#include <memory>

// Error handling policy: Quit, whenever an ALP error happens. ////////////////
#define VERIFY_ALP(AlpApiCall) { long Ret=AlpApiCall; if (ALP_OK!=Ret) { _tprintf( _T("ALP API Error Code %i (see alp.h) in function\r\n %s\r\nPress any key to exit."), Ret, _T(#AlpApiCall) ); _gettch(); return 1;} }

#define PRESS_KEY(strMessage) {	_tprintf( strMessage ); while (_kbhit()) _gettch(); _gettch();}

int _tmain(int /* argc */, _TCHAR* /* argv */[])
{
	_tprintf( _T("The ALP-DMD-Mask-Sample-Program demonstrates the function of the dmd-mask.\r\nIt will first show a mask (display of the mask is controllable by the user),\r\nlayed over a picture which shows the boundaries of the mask.\r\nIn a second step, the mask will be layed over a scrolling chess-board.\r\n------------------------------------------------------------------\r\n\r\n"));


// General declarations and definitions ///////////////////////////////////////
	ALP_ID AlpDevId = 0;
	ALP_ID AlpSeqId1 = 0;
	ALP_ID AlpSeqId2 = 0;
	bool bExit = false;
	bool bMaskOn = false;
	long nRows = 0;
	long nColumns = 0;
	long nFrames = 3;
	long nGridSize = 64;
	long nLineInc = 10;
	std::unique_ptr<tAlpDmdMask16K> AlpDmdMask(new tAlpDmdMask16K);
	long const nMaskBlockWidth = 16;
	long const nMaskBlockHeight = 16;
	long const ALP_DMD_MASK_Code = ALP_DMD_MASK_XY(nMaskBlockWidth,nMaskBlockHeight);

	AlpDmdMask->nBlockWidth = nMaskBlockWidth;
	AlpDmdMask->nRowCount = ALP_DEFAULT;
	AlpDmdMask->nRowOffset = ALP_DEFAULT;
	for(long i=0; i<16384; i++) AlpDmdMask->Bitmap[i] = 0x00;


// Initalize ALP //////////////////////////////////////////////////////////////
	VERIFY_ALP(AlpDevAlloc(0, 0, &AlpDevId));
	VERIFY_ALP(AlpDevInquire(AlpDevId, ALP_DEV_DISPLAY_HEIGHT, &nRows));
	VERIFY_ALP(AlpDevInquire(AlpDevId, ALP_DEV_DISPLAY_WIDTH, &nColumns));

	VERIFY_ALP(AlpSeqAlloc(AlpDevId, 1, 1, &AlpSeqId1));
	VERIFY_ALP(AlpSeqAlloc(AlpDevId, 1, nFrames, &AlpSeqId2));


// Compute sequence and mask-data /////////////////////////////////////////////
	std::unique_ptr<char unsigned[]> pSeqData1(new char unsigned[nRows*nColumns]);
	std::unique_ptr<char unsigned[]> pSeqData2(new char unsigned[nFrames*nRows*nColumns]);	

	long maskLeftBorder  =    nColumns/(4*16);
	long maskRightBorder = 3*(nColumns/(4*16));
	long maskUpperBorder =    nRows/(4*16);
	long maskLowerBorder = 3*(nRows/(4*16));

	// compute data for sequence 1 (borders around mask)
	for(long y=0; y < nRows; y++)
		for(long x=0; x < nColumns; x++)
		{
			if( x == 16*maskLeftBorder - 1 || x == 16*(maskRightBorder + 1) || y == 16*maskUpperBorder - 1 || y == 16*(maskLowerBorder + 1)  )
				pSeqData1[y*nColumns + x] = 0x00;
			else 
				pSeqData1[y*nColumns + x] = 0x80;
		}

	// compute mask-data (black rectangle in the middle of the dmd)
	for(long y=0; y < nRows/16+1; y++)
		for(long x=0; x < nColumns/16; x++)
		{
			if(maskLeftBorder > x || x > maskRightBorder || maskUpperBorder > y || y > maskLowerBorder)
				AlpDmdMask->Bitmap[(y*nColumns/16 + x) / 8] |= 0x01 << (7-(x%8));
		}

	// compute data for sequence 2 (chess-board)
	for(long y=0; y < nFrames * nRows; y++)
		for(long x=0; x < nColumns; x++)
		{
			if(0 == y/nGridSize % 2)
				pSeqData2[y*nColumns + x] = ((x/nGridSize) % 2) << 7;
			else
				pSeqData2[y*nColumns + x] = ((x/nGridSize + 1) % 2) << 7;
		}


// Prepare sequences and dmd-mask /////////////////////////////////////////////
	// load sequencedata
	VERIFY_ALP(AlpSeqPut(AlpDevId, AlpSeqId1, 0, 1, pSeqData1.get()));
	VERIFY_ALP(AlpSeqPut(AlpDevId, AlpSeqId2, 0, nFrames, pSeqData2.get()));

	// set up scrolling
	VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId2, ALP_SCROLL_FROM_ROW, 0));
	VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId2, ALP_SCROLL_TO_ROW, (nFrames-1) * nRows));
	VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId2, ALP_LINE_INC, nLineInc));

	// set up dmd-mask
	VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId1, ALP_DMD_MASK_SELECT, ALP_DMD_MASK_Code));
	VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId2, ALP_DMD_MASK_SELECT, ALP_DMD_MASK_Code));
	VERIFY_ALP(AlpProjControlEx(AlpDevId, ALP_DMD_MASK_WRITE_16K, (void*)AlpDmdMask.get()));

// Show the sequences /////////////////////////////////////////////////////////
	// start projection of sequence 1
	VERIFY_ALP(AlpProjStartCont(AlpDevId, AlpSeqId1));

	_tprintf( _T("1. Toggle the mask on and off with spacebar. Press any other key to continue.\r\n\r\n   mask is on \r") );
	
	// validate user-input
	while(!bExit)
	{
		if(_kbhit())
		{
			switch(_gettch())
			{
			case _T(' '):
				VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId1, ALP_DMD_MASK_SELECT, bMaskOn?ALP_DMD_MASK_Code:ALP_DEFAULT));
				VERIFY_ALP(AlpProjStartCont(AlpDevId, AlpSeqId1));
				
				if(bMaskOn)
					_tprintf( _T("   mask is on \r"));
				else
					_tprintf( _T("   mask is off\r"));

				bMaskOn = !bMaskOn;
				break;
			default:
				bExit = true;
			}
		}
	}

	// start projection of sequence 2 with scrolling
	VERIFY_ALP(AlpProjStartCont(AlpDevId, AlpSeqId2));

	PRESS_KEY(_T("2. Projecting scrolling chess-board with aktive mask.\r\n   Press any key to exit the programm."));

// Deallocate device //////////////////////////////////////////////////////////
	VERIFY_ALP(AlpDevHalt(AlpDevId));
	VERIFY_ALP(AlpDevFree(AlpDevId));

	return 0;
}