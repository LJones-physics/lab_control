// ALP FLUT Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "alp.h"
#include <memory>

#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_ESC 27

// Error handling policy: Quit, whenever an ALP error happens. ////////////////
#define VERIFY_ALP(AlpApiCall) { long Ret=AlpApiCall; if (ALP_OK!=Ret) { _tprintf( _T("ALP API Error Code %i (see alp.h) in function\r\n %s\r\nPress any key to exit."), Ret, _T(#AlpApiCall) ); _gettch(); return 1;} }

#define PRESS_KEY(strMessage) {	_tprintf( strMessage ); while (_kbhit()) _gettch(); _gettch();}

int _tmain(int /* argc */, _TCHAR* /* argv */[])
{
	_tprintf( _T("This ALP-FLUT-Sample-Program shows a sequence of 10 vertical \r\nstripes with tree different frame-orders:\r\n\r\n1. Linear. (order: 1, 2, 3, 4, 5, 6, 7, 8, 9, 10)\r\n2. With activated FLUT. (order: 1, 6, 2, 7, 3, 8, 4, 9, 5, 10 (back and forth))\r\n3. The user can modify the FLUT and watch the changes in realtime.\r\n------------------------------------------------------------------\r\n\r\n"));


// General declarations and definitions ///////////////////////////////////////
	ALP_ID AlpDevId = 0;
	ALP_ID AlpSeqId = 0;
	bool bExit = false;
	long nRows = 0;
	long nColumns = 0;
	long nFrames = 10;
	long nFrameIndex = 5;
	std::unique_ptr<tFlutWrite> pFlut(new tFlutWrite);


// Set up FLUT ////////////////////////////////////////////////////////////////
	pFlut->nOffset = 0;
	pFlut->nSize   = 2*nFrames;

	pFlut->FrameNumbers[ 0] = 0; 
	pFlut->FrameNumbers[ 1] = 5;
	pFlut->FrameNumbers[ 2] = 1;
	pFlut->FrameNumbers[ 3] = 6;
	pFlut->FrameNumbers[ 4] = 2;
	pFlut->FrameNumbers[ 5] = 7;
	pFlut->FrameNumbers[ 6] = 3;
	pFlut->FrameNumbers[ 7] = 8;
	pFlut->FrameNumbers[ 8] = 4;
	pFlut->FrameNumbers[ 9] = 9;

	for(long i=0; i<nFrames; i++)
		pFlut->FrameNumbers[nFrames+i] = pFlut->FrameNumbers[ nFrames-1-i];


// Initalize ALP //////////////////////////////////////////////////////////////
	VERIFY_ALP(AlpDevAlloc(0, 0, &AlpDevId));
	VERIFY_ALP(AlpDevInquire(AlpDevId, ALP_DEV_DISPLAY_HEIGHT, &nRows));
	VERIFY_ALP(AlpDevInquire(AlpDevId, ALP_DEV_DISPLAY_WIDTH, &nColumns));


// Set up Sequence ////////////////////////////////////////////////////////////
	std::unique_ptr<char unsigned[]> pSeqData(new char unsigned[nFrames*nRows*nColumns]);

	// generate a pattern of 10 vertical stripes
	for(long FrameCount = 0; FrameCount < nFrames; FrameCount++)
		for(long y=0; y < nRows; y++)
			for(long x=0; x < nColumns; x++)
			{
				if(FrameCount*nColumns/nFrames <= x && x < (FrameCount+1)*nColumns/nFrames)
					pSeqData[(FrameCount*nRows + y)*nColumns + x] = 0x00;
				else
					pSeqData[(FrameCount*nRows + y)*nColumns + x] = 0x80;
			}

	// transfere sequence into device
	VERIFY_ALP(AlpSeqAlloc(AlpDevId, 1, nFrames, &AlpSeqId));
	VERIFY_ALP(AlpSeqPut(AlpDevId, AlpSeqId, 0, nFrames, pSeqData.get()));
	VERIFY_ALP(AlpSeqTiming(AlpDevId, AlpSeqId, ALP_DEFAULT, 100000, ALP_DEFAULT, ALP_DEFAULT, ALP_DEFAULT ));


// Start projection without FLUT //////////////////////////////////////////////
	VERIFY_ALP(AlpProjStartCont(AlpDevId, AlpSeqId));

	PRESS_KEY(_T("1. Starting linear sequence-display. Press any key to continue.\r\n\r\n"));


// Start projection with FLUT /////////////////////////////////////////////////
	// halt device
	VERIFY_ALP(AlpDevHalt(AlpDevId));

	// transfere and activate FLUT
	VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId, ALP_FLUT_MODE, ALP_FLUT_9BIT));
	VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId, ALP_FLUT_ENTRIES9, 2*nFrames));
	VERIFY_ALP(AlpProjControlEx(AlpDevId, ALP_FLUT_WRITE_9BIT, pFlut.get()));

	// restart projection
	VERIFY_ALP(AlpProjStartCont(AlpDevId, AlpSeqId));

	PRESS_KEY(_T("2. Starting sequence-display with FLUT. Press any key to continue.\r\n\r\n"));


// Start projection with user-interactive FLUT /////////////////////////////////
	// halt device
	VERIFY_ALP(AlpDevHalt(AlpDevId));

	// use binary-uninterrupted with default-timing
	VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId, ALP_BIN_MODE, ALP_BIN_UNINTERRUPTED));
	VERIFY_ALP(AlpSeqTiming(AlpDevId, AlpSeqId, ALP_DEFAULT, ALP_DEFAULT, ALP_DEFAULT, ALP_DEFAULT, ALP_DEFAULT ));

	// restart projection
	VERIFY_ALP(AlpProjStartCont(AlpDevId, AlpSeqId));

	 _tprintf( _T("3. Starting sequence-display with user-interactive FLUT. \r\n   Use the left <- and right -> arrow-keys to shift the stripe.\r\n   Press spacebar to exit the programm.\r\n"));

	// verify user-input
	while(!bExit)
	{
		// rewrite FLUT
		for(long FlutFrameCount = 0; FlutFrameCount < 2*nFrames; FlutFrameCount++) pFlut->FrameNumbers[FlutFrameCount] = nFrameIndex; 
		VERIFY_ALP(AlpProjControlEx(AlpDevId, ALP_FLUT_WRITE_9BIT, pFlut.get()));

		if(_kbhit())
		{
			switch(_getch())
			{
			case KEY_ESC:
			case _T(' '):
				bExit = true;
				break;
			case KEY_LEFT:
				if(nFrameIndex > 0) nFrameIndex--;
				break;
			case KEY_RIGHT:
				if(nFrameIndex < nFrames-1) nFrameIndex++;
				break;
			default:;
			}	
		}

		if(bExit) break;	
	}

// Deallocate device //////////////////////////////////////////////////////////
	VERIFY_ALP(AlpDevHalt(AlpDevId));
	VERIFY_ALP(AlpDevFree(AlpDevId));

	return 0;
}