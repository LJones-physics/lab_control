// ALP Shearing Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "alp.h"
#include <memory>
#include <algorithm>
#include <math.h>

#define M_PI       3.14159265358979323846

// Error handling policy: Quit, whenever an ALP error happens. ////////////////
#define VERIFY_ALP(AlpApiCall) { long Ret=AlpApiCall; if (ALP_OK!=Ret) { _tprintf( _T("ALP API Error Code %i (see alp.h) in function\r\n %s\r\nPress any key to exit."), Ret, _T(#AlpApiCall) ); _gettch(); return 1;} }

#define PRESS_KEY(strMessage) {	_tprintf( strMessage ); while (_kbhit()) _gettch(); _gettch();}

int _tmain(int /* argc */, _TCHAR* /* argv */[])
{
	_tprintf( _T("The ALP-Shearing-Sample-Program will first show a scrolling chess-pattern with \r\nthree different (user selectable) shearing-configurations\r\n(off, linear shearing, sinus shearing).\r\nIn a second step, the chess-board is shifted vertical in a way,\r\nthat linear shearing will rotate the picture.\r\nIn this step, the user can toggle between linear shearing and no shearing.\r\n------------------------------------------------------------------\r\n\r\n"));


// General declarations and definitions ///////////////////////////////////////
	ALP_ID AlpDevId = 0;
	ALP_ID AlpSeqId1 = 0;
	ALP_ID AlpSeqId2 = 0;
	bool bExit = false;
	long shearIndex = 0;
	long nRows = 0;
	long nColumns = 0;
	long nFrames = 3;
	long nGridSize = 192;
	long nLineInc = 20;
	tAlpShearTable ShearTableLinear;
	tAlpShearTable ShearTableSinus;	


// Initialize ALP //////////////////////////////////////////////////////////////
	VERIFY_ALP(AlpDevAlloc(0, 0, &AlpDevId));
	VERIFY_ALP(AlpDevInquire(AlpDevId, ALP_DEV_DISPLAY_HEIGHT, &nRows));
	VERIFY_ALP(AlpDevInquire(AlpDevId, ALP_DEV_DISPLAY_WIDTH, &nColumns));

	VERIFY_ALP(AlpSeqAlloc(AlpDevId, 1, nFrames, &AlpSeqId1));
	VERIFY_ALP(AlpSeqAlloc(AlpDevId, 1, nFrames, &AlpSeqId2));


// Compute sequence and shear-data ////////////////////////////////////////////
	std::unique_ptr<char unsigned[]> pSeqData1(new char unsigned[nFrames*nRows*nColumns]);
	std::unique_ptr<char unsigned[]> pSeqData2(new char unsigned[nFrames*nRows*nColumns]);	


	// compute data for sequence 1 (chess-board)
	for(long y=0; y < nFrames * nRows; y++)
		for(long x=0; x < nColumns; x++)
		{
			if(0 == y/nGridSize % 2)
				pSeqData1[y*nColumns + x] = ((x/nGridSize) % 2) << 7;
			else
				pSeqData1[y*nColumns + x] = ((x/nGridSize + 1) % 2) << 7;
		}


	// compute data for sequence 2
	
	// a description of the used formulas can be found in the
	// "ALP-4.3 Shear Scrolling.pdf" documentation

	double const fTanAlpha = 511.0 / nRows;			// tangent of rotation angle Alpha - shear vertically and horizontally
	double const fAlpha = atan(fTanAlpha);			// rotation angle
	double const fCosAlpha = cos(fAlpha);			// cosine of rotation angle
	double const fSinAlpha = sin(fAlpha);			// sine of rotation angle
	long const nMaxShearDataRow = (long)(nFrames*nRows*fCosAlpha - nColumns*fTanAlpha);
	for(long y=0; y < nMaxShearDataRow; y++)
	{
		for(long x=0; x < nColumns; x++)
		{
			double const	fSrcX = x*fCosAlpha,
							fSrcY = y/fCosAlpha - x*fSinAlpha;
			long const	nSrcX = (long)(fSrcX),
						nSrcY = (long)(fSrcY);

			if (nSrcY<0 || nSrcY>=nFrames*nRows)
				pSeqData2[y*nColumns + x] = 0;	// area vertically outside  pSeqData1
			else
				pSeqData2[y*nColumns + x] = pSeqData1[nSrcY*nColumns + nSrcX];
		}
	}

	// set up shear-table 1 (linear shearing)			
	ShearTableLinear.nOffset = 0;
	ShearTableLinear.nSize = nRows;
	
	for(long i=0; i < nRows; i++)
		ShearTableLinear.nShiftDistance[i] = (long) (fTanAlpha*i);
	

	// set up shear-table 2 (sinus shearing) 
	double nPeriodPixels = (nRows/2);
	double fPeriodScale = 2*M_PI / nPeriodPixels;
	double fAmplitude = std::min( nPeriodPixels/2, 500. );

	ShearTableSinus.nOffset = 0;
	ShearTableSinus.nSize = nRows;

	for (long i=0; i < nRows; i++)
	{
		ShearTableSinus.nShiftDistance[i] = (long) (sin(-fPeriodScale*i) * fAmplitude/2 + fAmplitude/2);
		_ASSERT( ShearTableSinus.nShiftDistance[i] < 512 && ShearTableSinus.nShiftDistance[i] >= 0 );
	}


// Prepare sequences //////////////////////////////////////////////////////////
	// load sequence-data
	VERIFY_ALP(AlpSeqPut(AlpDevId, AlpSeqId1, 0, nFrames, pSeqData1.get()));
	VERIFY_ALP(AlpSeqPut(AlpDevId, AlpSeqId2, 0, nFrames, pSeqData2.get()));

	// set up scrolling
	VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId1, ALP_SCROLL_FROM_ROW, 0));
	VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId1, ALP_SCROLL_TO_ROW, (nFrames-1) * nRows));
	{
		long nPeriodSize;
		nPeriodSize = 2*nGridSize;
		// Optionally: Enable this command in order to avoid glitches in scrolling display:
		VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId2, ALP_SCROLL_TO_ROW, ((nFrames-1)*nRows)/nPeriodSize*nPeriodSize));
	}
	VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId1, ALP_LINE_INC, nLineInc));

	VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId2, ALP_SCROLL_FROM_ROW, 0));
	VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId2, ALP_SCROLL_TO_ROW, nMaxShearDataRow-nRows));
	{
		long nShearPeriodSize;
		nShearPeriodSize = (long) ((2*nGridSize)*fCosAlpha);	// rotated data has vertically smaller grid size
		// Enable this command in order to avoid glitches in scrolling display:
		VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId2, ALP_SCROLL_TO_ROW, (nMaxShearDataRow - nRows)/nShearPeriodSize*nShearPeriodSize));
	}
	
	VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId2, ALP_LINE_INC, nLineInc));


// Show the sequences /////////////////////////////////////////////////////////

	// Sequence 1
	_tprintf( _T("1. Toggle the shift-tables with spacebar. Press any other key to continue.\r\n\r\n") );
	
	// validate user-input
	while(!bExit)
	{
		switch(shearIndex)
		{
		case 0:
			VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId1, ALP_X_SHEAR_SELECT, ALP_DEFAULT));
			VERIFY_ALP(AlpDevHalt(AlpDevId));
			VERIFY_ALP(AlpProjStartCont(AlpDevId, AlpSeqId1));
			_tprintf( _T("   shearing is off            \r"));
			break;
		case 1:
			VERIFY_ALP(AlpProjControlEx(AlpDevId, ALP_X_SHEAR, &ShearTableLinear));
			VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId1, ALP_X_SHEAR_SELECT, ALP_ENABLE));
			VERIFY_ALP(AlpDevHalt(AlpDevId));
			VERIFY_ALP(AlpProjStartCont(AlpDevId, AlpSeqId1));
			_tprintf( _T("   linear shearing \r"));
			break;
		case 2:
			VERIFY_ALP(AlpProjControlEx(AlpDevId, ALP_X_SHEAR, &ShearTableSinus));
			_tprintf( _T("   sinus shearing \r"));
		}

		switch(_gettch())
		{
		case _T(' '):
			shearIndex = ( shearIndex + 1 ) % 3;
			break;
		default:
			bExit = true;
		}
	}

	// Sequence 2
	VERIFY_ALP(AlpProjControlEx(AlpDevId, ALP_X_SHEAR, &ShearTableLinear));
	_tprintf(_T("2. Projecting scrolling, shifted chess-board. \r\n   Toggle the shift on and off with spacebar. \r\n   Press any other key to exit the program.\r\n\r\n"));

	shearIndex = 1;
	bExit = false;

	// validate user-input
	while(!bExit)
	{
		switch(shearIndex)
		{
		case 0:
			VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId2, ALP_X_SHEAR_SELECT, ALP_DEFAULT));
			VERIFY_ALP(AlpDevHalt(AlpDevId));
			VERIFY_ALP(AlpProjStartCont(AlpDevId, AlpSeqId2));
			_tprintf( _T("   shearing is off            \r"));
			break;
		case 1:
			VERIFY_ALP(AlpSeqControl(AlpDevId, AlpSeqId2, ALP_X_SHEAR_SELECT, ALP_ENABLE));
			VERIFY_ALP(AlpDevHalt(AlpDevId));
			VERIFY_ALP(AlpProjStartCont(AlpDevId, AlpSeqId2));
			_tprintf( _T("   linear shearing (rotation) \r"));
			break;
		}

		switch(_gettch())
		{
		case _T(' '):
			shearIndex = ( shearIndex + 1 ) % 2;
			break;
		default:
			bExit = true;
		}
	}


// Deallocate device //////////////////////////////////////////////////////////
	VERIFY_ALP(AlpDevHalt(AlpDevId));
	VERIFY_ALP(AlpDevFree(AlpDevId));

	return 0;
}
