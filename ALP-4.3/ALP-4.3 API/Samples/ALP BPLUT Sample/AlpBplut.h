#pragma once
#include "alp.h"
#include <memory>
#include <string>

/** Manage memory buffer for ALP Bitplane Lookup Table (alp.h: tBplutWrite).

Implement algorithms to generate different kinds of bit-plane sequences, all with binary-weighted quantities (powers of two).
They can be extended to the required number of entries by duplicate() and distribute().
*/
class CAlpBplut
{
public:
	CAlpBplut(void);
	~CAlpBplut(void);

	/// getter function
	const tBplutWrite *getAlpBplut() const
	{
		return mBplut.get();
	}

	/// getter function
	const long getSize() const
	{ return mBplut->nSize; }

	/// Delta-sigma style high-frequency pattern of bit-planes.
	/// Example: a 4-bit pattern will be 15 values, [0 1 0 2 0 1 0 3 0 1 0 2 0 1 0]
	CAlpBplut& generateDeltaSigma(long const bitplaneCount);

	/// Pattern similar to ALP grayscale generation
	/// Example: a 4-bit pattern will be 15 values, [0 0 0 0 0 0 0 0 1 1 1 1 2 2 3]
	CAlpBplut& generateAlpStandard(long const bitplaneCount);

	/// Repeat each value blockSize times. This reduces frequency again.
	/// Note: Binary distribution of quantities is preserved as long as the size of mBplut is not exceeded!
	CAlpBplut& distribute(long const blockSize);

	/// Fill up to entryCount by duplicating the whole sequence. This keeps the frequency.
	/// Note: Relative quantities are preserved as long as entryCount is an integer multiple of the previous LUT size.
	CAlpBplut& duplicate(long const entryCount);

	/// Generate a string showing the LUT entries; truncate at maxBitplanes if required
	std::string tostring(long const maxBitplanes) const;

private:
	std::unique_ptr<tBplutWrite> mBplut;
};

