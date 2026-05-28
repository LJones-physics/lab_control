// ALP ImageConverter Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "atlstr.h"
#include "PlusGdi.h"	// GDI+
#include <memory>
#include <conio.h>
#include "shlwapi.h"
#include <iostream>
#include <fstream>

GDIPLUS_APP;									// start GDI+

void Pause();
CString getExeDir();
void CopyImageToOutputBufferBinaryTopDown(long nImageWidth, long nImageHeight, IN UCHAR* pInputBuffer, OUT UCHAR* pOutputBuffer);
inline UCHAR ConvertToBinaryTopDown(IN UCHAR* pInputBuffer);


int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		_tprintf( _T("ALP ImageConverter Sample.exe \r\n") );
		_tprintf( _T("This sample program scales and converts image files placed in a specified folder\r\ninto an ALP API data format and writes the output to a single file. \r\n") );
		_tprintf( _T("This program is intended to be used together with the ALP Streaming Sample program.\r\n") );
		_tprintf( _T("\r\nInput parameters: \r\n") );
		_tprintf( _T("1. Width of the source images (default=1024) \r\n") );
		_tprintf( _T("2. Height of the source images (default=768) \r\n") );
		_tprintf( _T("3. Height of the converted frames (default=768), must be <=\"Height of source image\".\r\n   If \"Height of the converted frames\" < \"Height of the source images\", the input images will be scaled.\r\n") );
		_tprintf( _T("4. Flip the image in vertical direction (default=0).\r\n") );
		_tprintf( _T("5. RGB mode. If set to a value other than 0, every input image will be split into 3 separate frames\r\n   containing only the red, green and blue image information (default=0). \r\n") );
		_tprintf( _T("6. ALP Data format of the converted image. If 0, the default ALP API data format ALP_DATA_MSB_ALIGN is used,\r\n   otherwise ALP_DATA_BINARY_TOPDOWN is used (default=0). See also chapter \"AlpSeqPut\" in the ALP-4 API description). \r\n") );
		_tprintf( _T("7. Path of input directory (default=directory of executable). If \"\" is specified, the default path is used.\r\n") );
		_tprintf( _T("8. Output file name (default=Output.ALP). If \"\" is specified, the default file name is used.\r\n   The file extension will be .ALP \r\n") );
		_tprintf( _T("\r\n") );


		// Set up variables for image dimensions and access /////////////////////////////
		const long nImageWidth = argc > 1 ? _ttoi(argv[1]) : 1024;
		const long nImageHeight = argc > 2 ? _ttoi(argv[2]) : 768;
		const long nImageHeightScaled = argc > 3 ? _ttoi(argv[3]) : 768;
		const long nImageWidthScaled = nImageWidth * nImageHeightScaled / nImageHeight;	// scale ImageWidth with the same factor as ImageHeight is scaled
		const long nImageWidthScaledOffset = (nImageWidth - nImageWidthScaled)/2;
		_tprintf( _T("Info:    Source Image Width: %i\r\n"), nImageWidth);	
		_tprintf( _T("Info:    Source Image Height: %i\r\n"), nImageHeight);		
		_tprintf( _T("Info:    Output Image Height: %i\r\n"), nImageHeightScaled);

		bool bFlipVertical = (argc > 4 && _ttoi(argv[4]) != 0);
		_tprintf( _T("Info:    FlipVertical: %i\r\n"), bFlipVertical);

		const long nColourComponents = (argc > 5 && _ttoi(argv[5]) != 0) ? 3 : 1;
		_tprintf( nColourComponents==1 ? _T("Info:    RGB mode: off\r\n") : _T("Info:    RGB mode: on\r\n"));

		const bool bDataFormatMsb = (argc <= 6 || _ttoi(argv[6]) == 0);

		long nOutputBufferSize = bDataFormatMsb ? /* Msb */ nImageWidth*nImageHeightScaled : /* Binary top down */ nImageWidth*nImageHeightScaled/8;
		std::unique_ptr<UCHAR[]> pOutputBuffer(new UCHAR[nOutputBufferSize]);


		// Set up IO file handling //////////////////////////////////////////////////////
		CString directoryNameLOAD = (argc > 7 && (CString)argv[7]!=_T("")) ? (CString)argv[7] : getExeDir();
		CString fileNameSAVE     = ((argc > 8 && (CString)argv[8]!=_T("")) ? (CString)argv[8] : _T("Output")) + _T(".ALP");
		_tprintf( _T("Info:    Input directory: \"") + directoryNameLOAD + _T("\"\r\n"));
		_tprintf( _T("Info:    Output file: \"") + fileNameSAVE + _T("\"\r\n"));
		_tprintf( _T("\r\n") );

		WIN32_FIND_DATA file;
		HANDLE hFileHandle = FindFirstFile(directoryNameLOAD + _T("\\*"), &file);
		std::ofstream outputFile(fileNameSAVE, std::ios::binary);		

		if(hFileHandle == INVALID_HANDLE_VALUE)
		{
			_tprintf( _T("Warning: No files found in the specified directory.\r\n"));
			_tprintf( _T("Done.\r\n"));
			Pause();
			return 0;	
		}
		
		if (!outputFile.is_open())
		{
			_tprintf( _T("Error:   Unable to open output file.\r\n"));
			Pause();
			return 0;	
		}

		Gdiplus::Status status = Gdiplus::Ok;
		long nConvertedFilesCounter = 0;


		// image processing	(derived from Easyproj) /////////////////////////////////////
		do
		{
			// Skip this item if it is a directory or the file extension is not an image format
			if( !( (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 && (
				PathMatchSpecW(file.cFileName, L"*.png") ||
				PathMatchSpecW(file.cFileName, L"*.bmp") ||
				PathMatchSpecW(file.cFileName, L"*.jpg") ||
				PathMatchSpecW(file.cFileName, L"*.jpeg"))))
				continue;

			Gdiplus::Bitmap BmpLoaded( directoryNameLOAD + _T("\\") + file.cFileName );			// read the image from file
			
			if( BmpLoaded.GetWidth() != (ULONG)nImageWidth || BmpLoaded.GetHeight() != (ULONG)nImageHeight)
			{
				_tprintf( _T("\r\nWarning: An image file found that does not match the specified dimensions, this file will be skipped.\r\n"));
				continue;
			}

			std::auto_ptr<Gdiplus::Bitmap> pBmpLoadedScaled(new Gdiplus::Bitmap( nImageWidth, nImageHeightScaled, PixelFormat24bppRGB));
			Gdiplus::Graphics ScaledGraphics( pBmpLoadedScaled.get() );

			// scale and convert image //////////////////////////////////////////////////
			for(int splitPicNum=0; splitPicNum < nColourComponents; splitPicNum++) // for RGB projection, a source-picture is split in to three target-pictures 
			{
				// create a 8bpp-sequence-image as bitmap
				std::auto_ptr<Gdiplus::Bitmap> pBmpConverted(new Gdiplus::Bitmap( nImageWidth, nImageHeightScaled, PixelFormat8bppIndexed));
			
				if( BmpLoaded.GetHeight() == pBmpLoadedScaled->GetHeight())
				{
					// resize not necessary -> clone the bitmap
					pBmpLoadedScaled.reset(BmpLoaded.Clone( 0, 0, nImageWidth, nImageHeight, PixelFormat24bppRGB));
				}
				else
				{
					// draw bitmap into the drawing area of the sequence image, resize
					status = ScaledGraphics.DrawImage( &BmpLoaded, nImageWidthScaledOffset, 0, nImageWidthScaled, nImageHeightScaled);
				}

				// lock RGB image for read access
				Gdiplus::Rect lockRect( 0, 0, pBmpLoadedScaled->GetWidth(), pBmpLoadedScaled->GetHeight());	
				Gdiplus::BitmapData bitmapDataScaled;		// image data
				pBmpLoadedScaled->LockBits( &lockRect, Gdiplus::ImageLockModeWrite, PixelFormat24bppRGB, &bitmapDataScaled);

				// lock 8bpp sequence image for write access
				Gdiplus::BitmapData bitmapData8bpp;		// image data
				pBmpConverted->LockBits( &lockRect, Gdiplus::ImageLockModeWrite, PixelFormat8bppIndexed, &bitmapData8bpp);

				BYTE *pImageDataScaled = static_cast<BYTE*>(bitmapDataScaled.Scan0);	// pointer to the first pixel of the first line (RGB image: source)
				BYTE *pImageData8bpp = static_cast<BYTE*>(bitmapData8bpp.Scan0);	// pointer to the first pixel of the first line (8bpp image: target)

				// transform pixel wise: RGB -> 8bpp
				if(3 == nColourComponents)						// RGB picture
				{
					// Byte-order of colour components: B,G,R
					// see also GdiPlusPixelFormats: BLUE_SHIFT=0, GREEN_SHIFT=8, RED_SHIFT=16
					int const nColorIndex = 2-splitPicNum;
					for( size_t y=0; y < pBmpLoadedScaled->GetHeight(); y++)
					{
						for( size_t x=0; x < pBmpLoadedScaled->GetWidth(); x++)
						{
							long index = bFlipVertical ? pBmpLoadedScaled->GetWidth() - x - 1 : x;

							pImageData8bpp[x] =	( pImageDataScaled[ index*3 + nColorIndex]);
						}
						pImageDataScaled += bitmapDataScaled.Stride;				// set pointer to the first pixel of the next line
						pImageData8bpp += bitmapData8bpp.Stride;				// set pointer to the first pixel of the next line
					}
				} else
				{	// monochrome
					for( size_t y=0; y < pBmpLoadedScaled->GetHeight(); y++)
					{
						for( size_t x=0; x < pBmpLoadedScaled->GetWidth(); x++)
						{
							long index = bFlipVertical ? pBmpLoadedScaled->GetWidth() - x - 1 : x;

							pImageData8bpp[x] =	( pImageDataScaled[ index*3]	// B
												+ pImageDataScaled[ index*3+1]	// G
												+ pImageDataScaled[ index*3+2]	// R
												) / 3;
						}
						pImageDataScaled += bitmapDataScaled.Stride;				// set pointer to the first pixel of the next line
						pImageData8bpp += bitmapData8bpp.Stride;				// set pointer to the first pixel of the next line
					}
				}
				

				// Write frame to file //////////////////////////////////////////////////
				pImageData8bpp = static_cast<BYTE*>(bitmapData8bpp.Scan0);	// set pointer back to the first pixel of the first line (RGB image: source)

				if(bDataFormatMsb)
					outputFile.write((char*)pImageData8bpp, nOutputBufferSize);
				else
				{
					CopyImageToOutputBufferBinaryTopDown( pBmpConverted->GetWidth(), pBmpConverted->GetHeight(), pImageData8bpp, pOutputBuffer.get());
					outputFile.write((char*)pOutputBuffer.get(), nOutputBufferSize);
				}

				
				if(status == Gdiplus::Ok)
					status = pBmpLoadedScaled->UnlockBits(&bitmapDataScaled);	// lock end

				if(status == Gdiplus::Ok)
					status = pBmpConverted->UnlockBits(&bitmapData8bpp);	// lock end
				
				if( status != Gdiplus::Ok)
				{
					_tprintf( _T("\r\nError:   Gdiplus function returned with code %i.\r\n"), status);
					outputFile.close();
					Pause();
					return 1;
				}
			}

			// report progress
			nConvertedFilesCounter++;
			_tprintf( _T("Info:    %i files converted.\r"), nConvertedFilesCounter);
			
		} while(FindNextFile(hFileHandle, &file));

		outputFile.close();
		_tprintf( _T("\r\nDone.\r\n"));
		Pause();
		return 0;
	}
	catch(...)
	{
		_tprintf( _T("\r\nError:   Exception thrown. This might be caused by insufficient memory or an invalid input parameter.\r\n"));
		Pause();
		return 1;			
	}
}

void Pause() 
{
	_tprintf( _T("Press any key") );
	while (_kbhit()) _gettch();	// clear keyboard buffer
	_gettch();
}

CString getExeDir()
{
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName( NULL ,buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	
	CString str = CString(buffer);
	str.Truncate(pos);
	return str;
}

void CopyImageToOutputBufferBinaryTopDown(long nImageWidth, long nImageHeight, IN UCHAR* pInputBuffer, OUT UCHAR* pOutputBuffer)
{
	UCHAR* pBuffer = pInputBuffer;

	for(long i=0; i < nImageWidth*nImageHeight/8; i++)
	{
		pOutputBuffer[i] = ConvertToBinaryTopDown(pBuffer);
		pBuffer = pBuffer + 8;
	}

	return;
}

// This function converts 8 Pixel to the ALP_DATA_BINARY_TOPDOWN data format of the ALP API
inline UCHAR ConvertToBinaryTopDown(IN UCHAR* pInputBuffer)
{
	return ((pInputBuffer[7] & 0x80) >> 7) |
		   ((pInputBuffer[6] & 0x80) >> 6) |
		   ((pInputBuffer[5] & 0x80) >> 5) |
		   ((pInputBuffer[4] & 0x80) >> 4) |
		   ((pInputBuffer[3] & 0x80) >> 3) |
		   ((pInputBuffer[2] & 0x80) >> 2) |
		   ((pInputBuffer[1] & 0x80) >> 1) |
		   ((pInputBuffer[0] & 0x80));
}