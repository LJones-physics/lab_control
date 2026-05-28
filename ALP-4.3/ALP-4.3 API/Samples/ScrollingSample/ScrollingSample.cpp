// ScrollingSample - Microsoft Win32 Console Application
//
// This is a part of the ALP application programming interface
// scrolling extension.
//
//	This sample is provided as-is, without any warranty.
//
//	Please always consult the ALP-4 API description when
//	customizing this program. It contains a detailled specification
//	of all Alp... functions.
//
// © 2008-2014 ViALUX GmbH. All rights reserved.
//
// Please see ReadMe.txt for details.

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <conio.h>
#include "alp.h"

void WaitForKeyStroke( LPCSTR strNote );
int ScrollingSample( long nLineInc );

int main( int /*argc*/, char ** /*argv*/ ) {
	if (ScrollingSample( +2 ) != 0)
	{
		WaitForKeyStroke( "Error. Press any Key." );
	}
}

int ScrollingSample( long nLineInc )
{
	/////////////////////////////////////////////
	// Set up test parameters

	// nFrames
	//		This number of frames is generated, transmitted, and
	//		displayed in this scrolling example.
	//		The image data represents two alternating frames:
	//		- 1st, 3rd, 5th... frame: shaded arrow
	//		- 2nd, 4th, 6th... frame: shaded circle
	const long nFrames = 4;	// number of frames in this sample

	//	SerialNumber
	//		use the specified ALP, 0=next free device
	long nSerial = 0;

	//	scrolling parameter set:
	//	LINE_INC: from Parameter nLineInc
	//	nFirstFrame and nLastFrame are converted into a scrolling row range (ALP_SCROLL_FROM_ROW, ALP_SCROLL_TO_ROW)
	//		Frame numbers are limited to 0..nFrames-1 (see nFrames above)
	long nFirstFrame = 0, nLastFrame = nFrames-1;

	//	ALP_BITNUM and ALP_PICTURE_TIME
	//		use this gray value depth (0 for binary uninterrupted)
	//		and picture time (µs)
	long nBitNum = 8, nPicTime = 10000;

	//	ALP_SEQ_REPEAT
	//		display the sequence n times, 0 for infinite until user interrupt
	long nRepeat = 0;

	/////////////////////////////////////////////
	// Initialize ALP
	ALP_ID nAlpId;
	long nDmdFormat;
	printf( "Initialize ALP..." );
	if (ALP_OK != AlpDevAlloc( nSerial, ALP_DEFAULT, &nAlpId ))
	{
		printf( "error.\r\n" );
		return 1;
	} else
		printf( "success.\r\n" );

	long nSizeX=0, nSizeY=0;
	if (ALP_OK != AlpDevInquire( nAlpId, ALP_DEV_DISPLAY_HEIGHT, &nSizeY ) ||
		ALP_OK != AlpDevInquire( nAlpId, ALP_DEV_DISPLAY_WIDTH, &nSizeX ) )
	{
		printf( "error: (inquire DMD format).\r\n" );
		return 1;
	}

	printf( "DMD format: %i*%i pixels\r\n", nSizeX, nSizeY );


	/////////////////////////////////////////////
	// Initialize sequence of 2 frames, 8 bits
	ALP_ID nSeqId;
	printf( "Initialize sequence and set up timing..." );
	if (ALP_OK != AlpSeqAlloc( nAlpId, 8, nFrames, &nSeqId ))
	{
		printf( "error (AlpSeqAlloc).\r\n" );
		return 1;
	}
	if (// set ALP_BITNUM
		ALP_OK != AlpSeqControl( nAlpId, nSeqId, ALP_BITNUM, 0==nBitNum?1:nBitNum ) ||
		// maybe set binary uninterrupted mode
		0==nBitNum &&
		ALP_OK != AlpSeqControl( nAlpId, nSeqId, ALP_BIN_MODE, ALP_BIN_UNINTERRUPTED ) ||
		// set up timing, also activating the ALP_BITNUM and ALP_BIN_MODE setting
		ALP_OK != AlpSeqTiming( nAlpId, nSeqId, ALP_DEFAULT, nPicTime, ALP_DEFAULT, ALP_DEFAULT, ALP_DEFAULT )
		)
	{
		printf( "error (timing settings).\r\n" );
		return 1;
	}
	if (// set ALP_SEQ_REPEAT
		nRepeat>0 &&
		ALP_OK != AlpSeqControl( nAlpId, nSeqId, ALP_SEQ_REPEAT, nRepeat ))
	{
		printf( "error (ALP_SEQ_REPEAT).\r\n" );
		return 1;
	} else
		printf( "success.\r\n" );

	/////////////////////////////////////////////
	// Check if scrolling is available, set up scrolling parameters
	if (ALP_OK != AlpSeqControl( nAlpId, nSeqId, ALP_LINE_INC, 0 ))
	{
		printf( "Scrolling is not available in this ALP API version!\r\n" );
		return 1;
	}
	printf( "Set up scrolling parameters..." );
	
	long const nScrollFromRow = nFirstFrame*nSizeY, nScrollToRow = nLastFrame*nSizeY;	// please adjust by offset, if required
	// parameters ALP_SCROLL_FROM_ROW and _TO_ROW set up scroll range
	// (they replace the old parameters ALP_FIRSTFRAME, ALP_LASTFRAME, ALP_FIRSTLINE, and ALP_LASTLINE)
	
	if (ALP_OK != AlpSeqControl( nAlpId, nSeqId, ALP_SCROLL_FROM_ROW, nScrollFromRow ) ||
		ALP_OK != AlpSeqControl( nAlpId, nSeqId, ALP_SCROLL_TO_ROW, nScrollToRow) )
	{
		printf( "error (scrolling range).\r\n" );
		return 1;
	}
	if (ALP_OK != AlpSeqControl( nAlpId, nSeqId, ALP_LINE_INC, nLineInc ) )
	{
		printf( "error (enable scrolling).\r\n" );
		return 1;
	}
	printf( "success.\r\n" );


	/////////////////////////////////////////////
	// Prepare and transmit image data: nFrames Images
	UCHAR *pImages = NULL;

	pImages = new UCHAR[nSizeX*nSizeY*nFrames];
	if (NULL == pImages) return 1;

	long nCurrent = 0, nX, nY;
	UCHAR nGray;

	// First frame (and all odd frames): shaded arrow (up direction)
	for (nY=0; nY<nSizeY; nY++) {	// loop through all lines of image data
		nGray = (UCHAR) (256*(nSizeY-1-nY)/nSizeY);
		for (nX=0; nX<nSizeX; nX++){// loop through all columns of image data

			// set pixel gray value depending on position (nX,nY)
			pImages[ (nCurrent*nSizeY+nY)*nSizeX + nX] =
				(nSizeX/2>nX+nY || nSizeX/2+nY<nX)
				? (UCHAR) 0			// nX (horizontal position) outside arrow shape
				: nGray;	// nX is inside arrow
		}
	}

	if (++nCurrent < nFrames)
	{
		// Second frame (and all even frames): a shaded circle
		long nRadius = nSizeY/2,
			nCenterX = nSizeX/2,
			nCenterY = nSizeY/2;
		long nSqrDistance;	// square distance (in pixels²) from center
		long nSqrRadius = nRadius*nRadius;
							// square radius (in pixels²) of the circle

		for (nY=0; nY<nSizeY; nY++) {
			for (nX=0; nX<nSizeX; nX++){
				nSqrDistance = (nCenterX-nX)*(nCenterX-nX) + (nCenterY-nY)*(nCenterY-nY);
				// set pixel gray value depending on position (nX,nY)
				if (nSqrDistance>nSqrRadius)
					pImages[ (nCurrent*nSizeY+nY)*nSizeX + nX] = 0;
				else {
					pImages[ (nCurrent*nSizeY+nY)*nSizeX + nX] =
						(UCHAR) (256*sqrt( (double) nSqrDistance )/(nRadius+1));
				}
			}
		}
	}
	while (++nCurrent < nFrames) {
		// odd image: copy from first one
		CopyMemory( pImages+nCurrent*nSizeY*nSizeX,
			pImages,
			nSizeY*nSizeX );
		if (++nCurrent == nFrames) break;

		// even image: copy from first one
		CopyMemory( pImages+nCurrent*nSizeY*nSizeX,
			pImages+nSizeY*nSizeX,
			nSizeY*nSizeX );
	}

	printf( "Transmit image data to ALP..." );
	if (ALP_OK != AlpSeqPut( nAlpId, nSeqId, ALP_DEFAULT, ALP_DEFAULT, pImages )) {
		printf( "error.\r\n" );
		return 1;
	} else
		printf( "success.\r\n" );

	delete[] pImages; pImages = NULL;

	/////////////////////////////////////////////
	// Run projection
	if (0 != nRepeat) {
		printf( "Start projection (%i times)...", nRepeat );
		if (ALP_OK != AlpProjStart( nAlpId, nSeqId ))
		{
			printf( "error (AlpProjStart).\r\n" );
			return 1;
		} else
			printf( "...started." );
		AlpProjWait( nAlpId );
		printf( "...finished.\r\n" );
	} else {
		printf( "Start projection (infinite times)..." );
		if (ALP_OK != AlpProjStartCont( nAlpId, nSeqId ))
		{
			printf( "error (AlpProjStartCont).\r\n" );
			return 1;
		} else
			printf( "...started.\r\n" );
	}

	/////////////////////////////////////////////
	// Clean
	WaitForKeyStroke( "Press return to fininsh.\r\n" );

	AlpDevHalt( nAlpId );
	AlpSeqFree( nAlpId, nSeqId );
	AlpDevFree( nAlpId );

	return 0;
}

// Wait for key stroke
void WaitForKeyStroke( LPCSTR strNote )
{
	if (strNote)
		printf( strNote );
	do { _getch(); } while (_kbhit()) ;
}
