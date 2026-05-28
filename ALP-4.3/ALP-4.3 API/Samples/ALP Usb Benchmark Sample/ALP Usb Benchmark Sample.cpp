#include <windows.h>
#include <TCHAR.h>
#include <atlstr.h>
#include <conio.h>
#include <memory>
#include "PlusGdi.h"	// GDI+

#include "alp.h"

GDIPLUS_APP;			// start GDI+

#define VERIFY_ALP(AlpApiCall) { long Ret=AlpApiCall; if (ALP_OK!=Ret) { _tprintf( _T("ALP API Error Code %i (see alp.h) in function\r\n %s\r\nPress any key to exit."), Ret, _T(#AlpApiCall) ); _gettch(); return 1;} }

int ConvertFrameData(CString fileNameLOAD, long nImageWidth, long nImageHeight, OUT UCHAR* pFrameBuffer, bool bRGB );


// define test parameters ///////////////////////////////////////////

/* Number of frames transferred with each call to AlpSeqPut(). Lower numbers will decrease performance.
If the value is very high, an exception might be thrown, due to missing main memory. 
Must be a multiple of 3 if an input data file is used and bFileConvertRgb is set to true. */
const ULONG nPicNum	= 600;

/* Number of calls to AlpSeqPut() for each test case. Higher numbers will narrowing the spreading of the test results. */
const ULONG nTestCount = 10;  

/* Activates a projection during the tests. Useful to simulate simultaneous projection and USB data transfer. */
const bool bProjection = false;

/* Only used if a file is passed as input argument.
Use this parameter if you want an input frame to be converted into three frames instead of one momocrome frame. */
const bool bFileConvertRgb = true;


int _tmain(int argc, _TCHAR* argv[])
{
	// define variables /////////////////////////////////////////////
	ALP_ID nDevId = ALP_INVALID_ID;
	ALP_ID nSeqId = ALP_INVALID_ID;

	const ULONG nBitplanes = 8;		// number of Bitplanes to be transferred

	std::unique_ptr<UCHAR[]> pDataBuffer;
	
	long nDmdHeight = 0;
	long nDmdWidth = 0;
	long nDataSize = 0;

	bool bFile = false;	// will be set to true later on, if a file is passed as input argument
	CString strInputFile = _T("no file");

	///////////////////////////////////////////////////////////////
	try {
		_tprintf( _T("Running \"Usb Benchmark Sample\".\r\nSee \"Usb Benchmark Sample.cpp\" for details.\r\n"));
		_tprintf( _T("\r\nThere are 3 test cases:\r\n	1. Pattern zero (complete black frame)\r\n	2. Pattern random (frame with pseudo random data)\r\n	3. Pattern file (test with user specified input file) (optional)\r\n"));

		if(argc>1)
		{
			bFile = true;
			strInputFile = argv[1];
		}
		else
			_tprintf( _T("\r\nNo custom image file loaded. If you want to load a custom image file, just drag and drop a *.tif;*.tiff;*.png;*.gif;*.bmp;*.jpg;*.jpeg to the \"Usb Benchmark Sample.exe\"\r\n\r\n") );


		_tprintf( _T("Initialize ALP...\r\n") );

		VERIFY_ALP(AlpDevAlloc( ALP_DEFAULT, ALP_DEFAULT, &nDevId));

		VERIFY_ALP(AlpDevInquire( nDevId, ALP_DEV_DISPLAY_HEIGHT, &nDmdHeight));
		VERIFY_ALP(AlpDevInquire( nDevId, ALP_DEV_DISPLAY_WIDTH, &nDmdWidth));
		
		nDataSize = nDmdHeight*nDmdWidth;
		pDataBuffer.reset(new UCHAR[nPicNum*nDataSize]);

		// Projection
		if(bProjection)
		{
			_tprintf( _T("Initialize Projection...\r\n") );
			ULONG nProjSeqId = 0;
			long nPROJ_PICTURE_TIME = 0;

			VERIFY_ALP(AlpSeqAlloc( nDevId, 1, 1, &nProjSeqId ));
			VERIFY_ALP(AlpSeqControl( nDevId, nProjSeqId, ALP_BIN_MODE, ALP_BIN_UNINTERRUPTED ));
			VERIFY_ALP(AlpSeqInquire( nDevId, nProjSeqId, ALP_MIN_PICTURE_TIME, &nPROJ_PICTURE_TIME ));
			VERIFY_ALP(AlpSeqTiming(nDevId, nProjSeqId, ALP_DEFAULT, nPROJ_PICTURE_TIME, ALP_DEFAULT, ALP_DEFAULT, ALP_DEFAULT ));
			VERIFY_ALP(AlpProjStartCont(nDevId, nProjSeqId));
		}


		// Benchmark tests
		{
			_tprintf( _T("Initialize Benchmark tests...\r\n") );
			_tprintf( _T("\r\nTest parameters:\r\n	nPicNum = %i\r\n	nTestCount = %i\r\n	bProjection = %i\r\n	File = %s\r\n	bFileConvertRgb = %i\r\n\r\n"), nPicNum, nTestCount, bProjection, strInputFile, bFileConvertRgb);
			_tprintf( _T("Test results:\r\n") );

			LARGE_INTEGER nFrequency, nStartTime, nEndTime;
			LONGLONG nTimeDiff;
			QueryPerformanceFrequency( &nFrequency );

			VERIFY_ALP(AlpSeqAlloc( nDevId, nBitplanes, nPicNum, &nSeqId));		


			// Pattern zero
			FillMemory( pDataBuffer.get(), nPicNum*nDataSize, 0 );
			
			// perform test
			QueryPerformanceCounter( &nStartTime );

			for(ULONG i=0; i < nTestCount; i++)
				VERIFY_ALP(AlpSeqPut(nDevId, nSeqId, 0, nPicNum, pDataBuffer.get()));

			QueryPerformanceCounter( &nEndTime );

			nTimeDiff = (nEndTime.QuadPart-nStartTime.QuadPart)*(LONGLONG)1000000 / nFrequency.QuadPart;
			_tprintf( _T("Pattern zero:	%3lli binary frames per second \r\n"),(LONGLONG) (nBitplanes*nPicNum*nTestCount)*1000000/nTimeDiff );


			// Pattern random
			for (ULONG i=0; i < nPicNum*nDataSize/sizeof(USHORT); i++)
				((USHORT*)pDataBuffer.get())[i] = (USHORT) rand();	// rand() returns values in the range of 0..32767 only
			
			// perform test
			QueryPerformanceCounter( &nStartTime );

			for(ULONG i=0; i < nTestCount; i++)
				VERIFY_ALP(AlpSeqPut(nDevId, nSeqId, 0, nPicNum, pDataBuffer.get()));

			QueryPerformanceCounter( &nEndTime );

			nTimeDiff = (nEndTime.QuadPart-nStartTime.QuadPart)*(LONGLONG)1000000 / nFrequency.QuadPart;
			_tprintf( _T("Pattern random:	%3lli binary frames per second \r\n"),(LONGLONG) (nBitplanes*nPicNum*nTestCount)*1000000/nTimeDiff );

			
			// Pattern file
			if(bFile)
			{
				// read input file and convert data
				if(ConvertFrameData(strInputFile, nDmdWidth, nDmdHeight, pDataBuffer.get(), bFileConvertRgb))
				{
					_tprintf( _T("Error during conversion of framedata. Press any key to exit.") );
					_getch(); while (_kbhit()) _getch();
					return 1;
				}

				if(bFileConvertRgb)
				{
					if(0 != (nPicNum % 3))
						_tprintf( _T("Error: nPicNum must be multiple of 3 if a framedata input file is used and bFileConvertRgb is set to true.") );

					for(ULONG i=0; i < nPicNum/3; i++)
						memcpy(pDataBuffer.get()+3*i*nDataSize, pDataBuffer.get(), 3*nDataSize);			
				}
				else
				{
					for(ULONG i=0; i < nPicNum; i++)
						memcpy(pDataBuffer.get()+i*nDataSize, pDataBuffer.get(), nDataSize);
				}
				
				// perform test 
				QueryPerformanceCounter( &nStartTime );

				for(ULONG i=0; i < nTestCount; i++)
					VERIFY_ALP(AlpSeqPut(nDevId, nSeqId, 0, nPicNum, pDataBuffer.get()));

				QueryPerformanceCounter( &nEndTime );

				nTimeDiff = (nEndTime.QuadPart-nStartTime.QuadPart)*(LONGLONG)1000000 / nFrequency.QuadPart;
				_tprintf( _T("Pattern file:	%3lli binary frames per second \r\n"),(LONGLONG) (nBitplanes*nPicNum*nTestCount)*1000000/nTimeDiff );
			}
			
		}
	} catch( ... ) {
		_tprintf( _T("Error: exception during test. This might be caused by insufficient main memory. Try to reduce nPicNum.\r\nPress any key to exit.") );
		_getch(); while (_kbhit()) _getch();
		return 1;
	}

	_tprintf( _T("\r\nTest done. Press any key to exit.") );
	_getch(); while (_kbhit()) _getch();

	_tprintf( _T("\n\n") );

	AlpDevHalt( nDevId );
	AlpDevFree ( nDevId );
	
	return 0;
}

int ConvertFrameData(CString fileNameLOAD, long nImageWidth, long nImageHeight, OUT UCHAR* pFrameBuffer, bool bRGB )
{
	/* copied from EasyProj */


	// read images from file, convert it and put it to the outputbuffer
	Gdiplus::Status status;	
	Gdiplus::Bitmap BmpLoaded( fileNameLOAD);			// read the image from file

	int nColourComponents = bRGB ? 3 : 1;

	try
	{
		// create a new bitmap with dmd dimensions
		std::unique_ptr<Gdiplus::Bitmap> pBmp(new Gdiplus::Bitmap( nImageWidth, nImageHeight, PixelFormat24bppRGB));
		Gdiplus::Graphics Graphics( pBmp.get() );

		for(int splitPicNum=0; splitPicNum < nColourComponents; splitPicNum++) // for RGB, a source-picture is splited in to three target-pictures 
		{
			// create a 8bpp-sequence-image as bitmap with dmd dimensions
			std::unique_ptr<Gdiplus::Bitmap> pSeqImageBmp(new Gdiplus::Bitmap( nImageWidth, nImageHeight, PixelFormat8bppIndexed));

			if( BmpLoaded.GetWidth() == pBmp->GetWidth()
				&&	BmpLoaded.GetHeight() == pBmp->GetHeight())
			{
				// resize not necessary -> clone the bitmap
				pBmp.reset(BmpLoaded.Clone( 0, 0, nImageWidth, nImageHeight, PixelFormat24bppRGB));
			}
			else
			{
				// draw bitmap into the drawing area of the sequence image, resize
				status = Graphics.DrawImage( &BmpLoaded, 0, 0, nImageWidth, nImageHeight);
			}

			if( pBmp->GetWidth() != pSeqImageBmp->GetWidth()
				||	pBmp->GetHeight() != pSeqImageBmp->GetHeight())
				throw;

			// lock RGB image for read access
			Gdiplus::Rect lockRect( 0, 0, pBmp->GetWidth(), pBmp->GetHeight());	
			Gdiplus::BitmapData bitmapData;		// image data
			pBmp->LockBits( &lockRect, Gdiplus::ImageLockModeWrite, PixelFormat24bppRGB, &bitmapData);

			// lock 8bpp sequence image for write access
			Gdiplus::BitmapData bitmapData8bpp;		// image data
			pSeqImageBmp->LockBits( &lockRect, Gdiplus::ImageLockModeWrite, PixelFormat8bppIndexed, &bitmapData8bpp);

			BYTE *pImageData = static_cast<BYTE*>(bitmapData.Scan0);	// pointer to the first pixel of the first line (RGB image: source)
			BYTE *pImageData8bpp = static_cast<BYTE*>(bitmapData8bpp.Scan0);	// pointer to the first pixel of the first line (8bpp image: target)

			// transform pixel wise: RGB -> 8bpp
			if(3 == nColourComponents)						// RGB picture
			{
				// Byte-order of color components: B,G,R
				// see also GdiPlusPixelFormats: BLUE_SHIFT=0, GREEN_SHIFT=8, RED_SHIFT=16
				int const nColorIndex = 2-splitPicNum;
				for( size_t y=0; y < pBmp->GetHeight(); y++)
				{
					for( size_t x=0; x < pBmp->GetWidth(); x++)
					{
						pImageData8bpp[x] =	( pImageData[ x*3 + nColorIndex]);
					}
					pImageData += bitmapData.Stride;				// set pointer to the first pixel of the next line
					pImageData8bpp += bitmapData8bpp.Stride;				// set pointer to the first pixel of the next line
				}
			} else
			{	// monochrome
				for( size_t y=0; y < pBmp->GetHeight(); y++)
				{
					for( size_t x=0; x < pBmp->GetWidth(); x++)
					{
						pImageData8bpp[x] =	( pImageData[ x*3]		// B
											+ pImageData[ x*3+1]	// G
											+ pImageData[ x*3+2]	// R
											) / 3;
					}
					pImageData += bitmapData.Stride;				// set pointer to the first pixel of the next line
					pImageData8bpp += bitmapData8bpp.Stride;				// set pointer to the first pixel of the next line
				}
			}

			pImageData8bpp = static_cast<BYTE*>(bitmapData8bpp.Scan0);	// set pointer back to the first pixel of the first line (RGB image: source)

			memcpy(pFrameBuffer+splitPicNum*nImageWidth*nImageHeight, pImageData8bpp, pSeqImageBmp->GetWidth()*pSeqImageBmp->GetHeight());	// write image to frame buffer

			status = pBmp->UnlockBits(&bitmapData);	// lock end
			status = pSeqImageBmp->UnlockBits(&bitmapData8bpp);	// lock end
		}
	} catch(...) {
		return 1;
	}

	return 0;
}