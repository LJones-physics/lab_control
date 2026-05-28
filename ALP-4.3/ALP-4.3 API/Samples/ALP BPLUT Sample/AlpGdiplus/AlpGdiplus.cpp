/** (c) 2023 ViALUX GmbH. All rights reserved. */

#include "AlpGdiplus.h"
#include <memory>

namespace AlpGdiplus
{
/** \brief convert a Gdiplus Bitmap to ALP format required by AlpSeqPut
* \param SourceBmp data source, can be loaded directly from file, e.g. Gdiplus::Bitmap(filename)
* \param pDestAlpBuffer destination buffer, must have a size of at least height*width bytes
* \param width ALP picture width (usually same as DMD width in pixels). Source bitmap is scaled to this width, if required.
* \param height ALP picture height (usually DMD height). Source bitmap is scaled to this height, if required. Aspect ratio is not preserved.
*/
bool convert(Gdiplus::Bitmap &SourceBmp, BYTE *const pDestAlpBuffer, unsigned int const width, unsigned int const height)
{
	Gdiplus::Status status;

	// create a new bitmap with DMD dimensions
	std::unique_ptr<Gdiplus::Bitmap> pProjBmp(new Gdiplus::Bitmap( width, height, PixelFormat24bppRGB));
	Gdiplus::Graphics projGraphics( pProjBmp.get() );

	if( SourceBmp.GetWidth() == pProjBmp->GetWidth()
		&&	SourceBmp.GetHeight() == pProjBmp->GetHeight())
	{
		// resize not necessary -> clone the bitmap
		pProjBmp.reset(SourceBmp.Clone( 0, 0, width, height, PixelFormat24bppRGB));
	}
	else
	{
		// draw bitmap into the drawing area of the sequence image, resize
		status = projGraphics.DrawImage( &SourceBmp, 0, 0, width, height);
	}

	if( pProjBmp->GetWidth() != width
		||	pProjBmp->GetHeight() != height)
		return false;

	// lock RGB image for read access
	Gdiplus::Rect lockRect( 0, 0, width, height);	
	Gdiplus::BitmapData bitmapDataProj;		// image data
	pProjBmp->LockBits( &lockRect, Gdiplus::ImageLockModeWrite, PixelFormat24bppRGB, &bitmapDataProj);

	BYTE *pImageDataProj = static_cast<BYTE*>(bitmapDataProj.Scan0);	// pointer to the first pixel of the first line (RGB image: source)	
	BYTE *pImageData8bpp = pDestAlpBuffer;								// pointer to the first pixel of the first line (8bpp image: target)

	// transform pixel wise: RGB -> 8bpp
	for( size_t y=0; y < height; y++)
	{
		for( size_t x=0; x < width; x++)
		{
			pImageData8bpp[x] =	( pImageDataProj[ x*3]		// B
								+ pImageDataProj[ x*3+1]	// G
								+ pImageDataProj[ x*3+2]	// R
								) / 3;
		}
		pImageDataProj += bitmapDataProj.Stride;				// set pointer to the first pixel of the next line
		pImageData8bpp += width;								// set pointer to the first pixel of the next line
	}

	status = pProjBmp->UnlockBits(&bitmapDataProj);	// lock end
	return true;
}

/** \brief same as convert(), but extract three color components, one ALP buffer for red, green, and blue
**/
bool convertRgb(Gdiplus::Bitmap &SourceBmp, BYTE *const pDestAlpBufferRed, BYTE *const pDestAlpBufferGreen, BYTE *const pDestAlpBufferBlue, unsigned int const width, unsigned int const height)
{
	Gdiplus::Status status;

	// create a new bitmap with DMD dimensions
	std::auto_ptr<Gdiplus::Bitmap> pProjBmp(new Gdiplus::Bitmap( width, height, PixelFormat24bppRGB));
	Gdiplus::Graphics projGraphics( pProjBmp.get() );

	if( SourceBmp.GetWidth() == pProjBmp->GetWidth()
		&&	SourceBmp.GetHeight() == pProjBmp->GetHeight())
	{
		// resize not necessary -> clone the bitmap
		pProjBmp.reset(SourceBmp.Clone( 0, 0, width, height, PixelFormat24bppRGB));
	}
	else
	{
		// draw bitmap into the drawing area of the sequence image, resize
		status = projGraphics.DrawImage( &SourceBmp, 0, 0, width, height);
	}

	if( pProjBmp->GetWidth() != width
		||	pProjBmp->GetHeight() != height)
		return false;

	// lock RGB image for read access
	Gdiplus::Rect lockRect( 0, 0, width, height);	
	Gdiplus::BitmapData bitmapDataProj;		// image data
	pProjBmp->LockBits( &lockRect, Gdiplus::ImageLockModeWrite, PixelFormat24bppRGB, &bitmapDataProj);

	for(int splitPicNum=0; splitPicNum<3; splitPicNum++) // for RGB projection, a source-picture is splited in to three target-pictures 
	{
		BYTE *pImageDataProj = static_cast<BYTE*>(bitmapDataProj.Scan0);	// pointer to the first pixel of the first line (RGB image: source)	
		BYTE *pImageData8bpp =												// pointer to the first pixel of the first line (8bpp image: target)
			splitPicNum==0?pDestAlpBufferRed:
			splitPicNum==1?pDestAlpBufferGreen:
			splitPicNum==2?pDestAlpBufferBlue:
			nullptr;

		// transform pixel wise: RGB -> 8bpp
		// Byte-order of color components: B,G,R
		// see also GdiPlusPixelFormats: BLUE_SHIFT=0, GREEN_SHIFT=8, RED_SHIFT=16
		int const nColorIndex = 2-splitPicNum;
		for( size_t y=0; y < height; y++)
		{
			for( size_t x=0; x < width; x++)
			{
				pImageData8bpp[x] =	( pImageDataProj[ x*3 + nColorIndex]);
			}
			pImageDataProj += bitmapDataProj.Stride;				// set pointer to the first pixel of the next line
			pImageData8bpp += width;								// set pointer to the first pixel of the next line
		}
	}
	status = pProjBmp->UnlockBits(&bitmapDataProj);	// lock end
	return true;
}

};	// namespace AlpGdiplus