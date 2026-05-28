#define NOMINMAX
#include "AlpBplut.h"

#include <algorithm>
using namespace std;

CAlpBplut::CAlpBplut(void) :
	mBplut(new tBplutWrite)
{
	mBplut->nOffset = mBplut->nSize = 0;
}


CAlpBplut::~CAlpBplut(void)
{
}

std::string CAlpBplut::tostring(long const maxBitplanes) const
{
	string result;
	long const maxIndex = min(min(maxBitplanes, mBplut->nSize), (long)_countof(mBplut->BitPlanes))-1;
	for (long i=0; i<maxIndex; i++)
		result += std::to_string((_ULonglong) mBplut->BitPlanes[i]) + " ";
	result += std::to_string((_ULonglong) mBplut->BitPlanes[maxIndex]);
	if (maxIndex < mBplut->nSize-1)
		result += "...";
	return result;
}

CAlpBplut& CAlpBplut::generateDeltaSigma(long const bitplaneCount)
{
	long entryCount = (1<<bitplaneCount) - 1;	/// = number of gray-scale values that can be encoded using bitplaneCount bits
	if (entryCount>_countof(mBplut->BitPlanes))
		entryCount = _countof(mBplut->BitPlanes);

	mBplut->nOffset = 0;
	mBplut->nSize = entryCount;

	// Write bit-planes to the memory in a high-frequency pattern, beginning with the most-significant (bit plane 0 in ALP).
	unsigned char bitplane=0;
	long firstIndex=0;
	long stepWidth=2;	//< The stepWidth increases while bit weight decreases.

	while (firstIndex<entryCount)
	{
		// Skip throught the LUT by stepWidth, processing all entries for the current bitplane:
		for (long i=firstIndex; i<entryCount; i+=stepWidth)
		{
			// write a block of same bit-planes
			mBplut->BitPlanes[i] = bitplane;
		}

		// Next bitplane entries start right in the middle of the first "gap", and have the double stepWidth.
		firstIndex += stepWidth/2;
		stepWidth *= 2;
		bitplane++;
	}

	return *this;
}

CAlpBplut& CAlpBplut::generateAlpStandard(long const bitplaneCount)
{
	long entryCount = (1<<bitplaneCount) - 1;	/// = number of gray-scale values that can be encoded using bitplaneCount bits
	if (entryCount>_countof(mBplut->BitPlanes))
		entryCount = _countof(mBplut->BitPlanes);

	mBplut->nOffset = 0;
	mBplut->nSize = entryCount;

	long weight=1<<(bitplaneCount-1);
	long firstIndex = 0;
	// assign all bitplanes, MSB (=0) first:
	for (long bitplane=0; bitplane<bitplaneCount; bitplane++)
	{
		for (long i=0; i<weight; i++)
			mBplut->BitPlanes[firstIndex+i] = (unsigned short) bitplane;

		firstIndex += weight;
		weight /= 2;
	}

	return *this;
}

CAlpBplut& CAlpBplut::distribute(long const blockSize)
{
	tBplutWrite const origin = *mBplut;

	// Increase size of BPLUT:
	mBplut->nSize *= blockSize;
	if (mBplut->nSize > _countof(mBplut->BitPlanes))
		mBplut->nSize = _countof(mBplut->BitPlanes);

	// duplicate entries from origin multiple times to mBplut:
	for (long i=0; i<origin.nSize; i++)
	{
		for (long j=0; j<blockSize; j++)
		{
			if (i*blockSize + j >= mBplut->nSize)
				return *this;	// warning: size is exceeded, some original values are lost

			mBplut->BitPlanes[i*blockSize + j] = origin.BitPlanes[i];
		}
	}

	return *this;
}

CAlpBplut& CAlpBplut::duplicate(long const entryCount)
{
	tBplutWrite const origin = *mBplut;

	// Increase size of BPLUT:
	mBplut->nSize = entryCount;
	if (mBplut->nSize > _countof(mBplut->BitPlanes))
		mBplut->nSize = _countof(mBplut->BitPlanes);

	if (origin.nSize == 0)
	{
		// cannot duplicate, because there is nothing generated before...
		for (long i=0; i<entryCount; i++)
			mBplut->BitPlanes[i] = 0;
		return *this;
	}

	for (long i=0; i<mBplut->nSize; i++)
	{
		mBplut->BitPlanes[i] = origin.BitPlanes[i % origin.nSize];
	}

	return *this;
}
