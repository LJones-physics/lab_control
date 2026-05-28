/** (c) 2023 ViALUX GmbH. All rights reserved. */

#pragma once

#include <Windows.h>
#include <gdiplus.h>

namespace AlpGdiplus
{
/** \brief convert a Gdiplus Bitmap to ALP format required by AlpSeqPut
* \param SourceBmp data source, can be loaded directly from file, e.g. Gdiplus::Bitmap(filename)
* \param pDestAlpBuffer destination buffer, must have a size of at least height*width bytes
* \param width ALP picture width (usually same as DMD width in pixels). Source bitmap is scaled to this width, if required.
* \param height ALP picture height (usually DMD height). Source bitmap is scaled to this height, if required. Aspect ratio is not preserved.
* \return true on success, false on error
*/
bool convert(Gdiplus::Bitmap &SourceBmp, BYTE *const pDestAlpBuffer, unsigned int const width, unsigned int const height);

/** \brief same as convert(), but extract three color components, one ALP buffer for red, green, and blue
**/
bool convertRgb(Gdiplus::Bitmap &SourceBmp, BYTE *const pDestAlpBufferRed, BYTE *const pDestAlpBufferGreen, BYTE *const pDestAlpBufferBlue, unsigned int const width, unsigned int const height);

};	// namespace AlpGdiplus