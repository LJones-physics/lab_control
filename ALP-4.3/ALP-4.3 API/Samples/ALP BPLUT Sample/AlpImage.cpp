#include "AlpImage.h"
#include "AlpGdiplus/GdiplusApplication.h"
#include "AlpGdiplus/AlpGdiplus.h"
using namespace Gdiplus;
using namespace std;

GDIPLUS_APP;

CAlpImage::CAlpImage(long nDmdWidth, long nDmdHeight)
	: mAlpSeqSize(nDmdWidth, nDmdHeight, 0, 0, 0)
{
	unload();
}

CAlpImage::~CAlpImage(void)
{
	unload();
}

bool CAlpImage::load( const std::string &sFilename )
{
	unload();

	// Load an image file to a memory buffer suitable for AlpSeqPut:

	// 1. Determine dimensions of image file
	wstring wFilename = wstring(sFilename.begin(), sFilename.end());	// GDI+ requires wide character strings
	unique_ptr<Bitmap> img(new Bitmap(wFilename.c_str()));
	if (img->GetLastStatus() != Ok)
		return false;

	UINT const imgWidth = img->GetWidth();
	UINT const imgHeight = img->GetHeight();

	// 2. calculate payload size and required memory buffer size (width, height including black areas)
	// Consider img as a stripe and scale to DMD width
	double const scaleFactor = (double)getDmdWidth() / imgWidth;
	long const payloadHeight = (long) (imgHeight * scaleFactor);
	mAlpSeqSize = CAlpSeqSize( getDmdWidth(), getDmdHeight(), payloadHeight, getTopFence(), getBottomFence() );
	// allocate memory buffer for payload, initialize black area
	mData.reset(mAlpSeqSize.newBuffer());

	// 3. call IML to load the image file into this memory buffer
	if (getDmdWidth()>USHRT_MAX || getPayloadHeight()>USHRT_MAX)
		return false;	// Restriction of IML. ALP can handle larger sequences. If required join data by multiple AlpSeqPut calls.

	if (! AlpGdiplus::convert(*img, getPayloadData(), getDmdWidth(), getPayloadHeight()))
		return false;

	mFilename = sFilename;
	return true;
}

void CAlpImage::unload()
{
	mAlpSeqSize = CAlpSeqSize( getDmdWidth(), getDmdHeight(), 0, getTopFence(), getBottomFence() );
	mData.reset();
	mFilename.clear();
}

CAlpImage::CAlpSeqSize::CAlpSeqSize(long const width,
			long const dmdHeight, long const payloadHeight,
			long topFence, long bottomFence) :
	mDmdWidth(width),
	mDmdHeight(dmdHeight),
	mPayloadHeight(payloadHeight),
	mTopFenceRows(topFence),
	mBottomFenceRows(bottomFence)
{
	/* add fences of black rows before and after payload */
	mSequenceHeight = payloadHeight + mTopFenceRows + mBottomFenceRows;
	/* round up sequence height to multiple of DMD heights */
	mSequencePictures = (mSequenceHeight + mDmdHeight-1)/mDmdHeight;
	mSequenceHeight = mSequencePictures*mDmdHeight;
}

unsigned char * CAlpImage::CAlpSeqSize::newBuffer() const
{
	unique_ptr<unsigned char[]> buffer(new unsigned char[mDmdWidth*mSequenceHeight]);

	// clear top fence
	unsigned char *const pBuffer = buffer.get();
	long row=0;
	ZeroMemory( pBuffer + row*mDmdWidth, mDmdWidth*mTopFenceRows );
	// step to bottom fence and clear it down to the end of sequence (maybe beyond fence, due to padding)
	row += mTopFenceRows + mPayloadHeight;
	ZeroMemory( pBuffer + row*mDmdWidth, mDmdWidth*(mSequenceHeight-row) );

	return buffer.release();
}

CAlpImage& CAlpImage::setTopFence(long nRows)
{
	CAlpSeqSize originalSize = mAlpSeqSize;
	unique_ptr<unsigned char[]> originalData(mData.release());

	// take over all but top-fence from original
	mAlpSeqSize = CAlpSeqSize( getDmdWidth(), getDmdHeight(), getPayloadHeight(), nRows, getBottomFence() );

	CopyMemory(
		getPayloadData(),
		originalData.get() + originalSize.getPayloadDataOffset(),
		getPayloadHeight()*getDmdWidth() );

	return *this;
}

CAlpImage& CAlpImage::setBottomFence(long nRows)
{
	CAlpSeqSize originalSize = mAlpSeqSize;
	unique_ptr<unsigned char[]> originalData(mData.release());

	// take over all but bottom-fence from original
	mAlpSeqSize = CAlpSeqSize( getDmdWidth(), getDmdHeight(), getPayloadHeight(), getTopFence(), nRows );

	CopyMemory(
		getPayloadData(),
		originalData.get() + originalSize.getPayloadDataOffset(),
		getPayloadHeight()*getDmdWidth() );

	return *this;
}
