#pragma once

#include <string>
#include <memory>

/** Manage an ALP Image: read from file to memory buffer, add black areas (for scrolling)
*/
class CAlpImage
{
	/// hide default constructor
	CAlpImage(); // = delete;
public:
	CAlpImage(long nDmdWidth, long nDmdHeight);
	~CAlpImage(void);
	
	/** Read image payload data from a file.
	Create a memory buffer storing an integer multiple of mDmdHeight rows (round up image size).
	Add fence rows, if already specified.
	*/
	bool load( const std::string &sFilename );

	/// Remove memory buffer
	void unload();

	/** Fence: add a number of black rows on top and/or bottom of the payload image data.
	This allows for completely scrolling all image rows over the DMD.
	*/
	CAlpImage& setTopFence( long nRows );
	CAlpImage& setBottomFence( long nRows );

	/// getter function
	inline long getDmdWidth() const
	{ return mAlpSeqSize.mDmdWidth; }

	/// getter function
	inline long getDmdHeight() const
	{ return mAlpSeqSize.mDmdHeight; }

	/// getter function
	inline long getSequenceHeight() const
	{ return mAlpSeqSize.mSequenceHeight; }

	/// getter function
	inline long getPayloadHeight() const
	{ return mAlpSeqSize.mPayloadHeight; }

	/// getter function
	inline long getTopFence() const
	{ return mAlpSeqSize.mTopFenceRows; }

	/// getter function
	inline long getBottomFence() const
	{ return mAlpSeqSize.mBottomFenceRows; }

	/// getter function
	long getSequencePictures() const
	{ return mAlpSeqSize.mSequencePictures; }

	/** getter function
	\return Pointer to whole memory buffer
	*/
	unsigned char* getSequenceData()
	{ return mData.get(); }

	/** getter function.
	\return Pointer _inside_ memory buffer, pointing to real image data, i.e. after the top fence area.
	*/
	unsigned char* getPayloadData()
	{
		return mData.get()
			?mData.get()+mAlpSeqSize.getPayloadDataOffset()
			:nullptr;
	}

private:
	std::string mFilename;

	/// Encapsulate size calculations and allocate memory buffer.
	class CAlpSeqSize
	{
	public:
		/// Calculate required mSequenceHeight for payloadHeight, taking fences and mDmdHeight into account.
		CAlpSeqSize( long const width,
			long const dmdHeight, long const payloadHeight,
			long topFence, long bottomFence );

		/// new[] memory buffer, write zero to fence and padding area
		unsigned char* newBuffer() const;

		/// calculate offset to access the payload data inside a memory buffer
		long getPayloadDataOffset() const
		{ return mDmdWidth*mTopFenceRows; }

	private:
		friend class CAlpImage;

		/// DMD dimensions; this is the processing quantum of AlpSeqPut
		long mDmdWidth, mDmdHeight;

		/// number of image rows
		long mPayloadHeight;

		/// Fences: black areas for scrolling:
		/// mTopFence serves as an offset in mDmdBuffer, where payload data begins
		/// mBottomFenceRows are added below payload
		long mTopFenceRows, mBottomFenceRows;

		/// Sequence height is mTopFenceRows + mPayloadHeight + mBottomFenceRows
		long mSequenceHeight;

		/// Sequence size in the original ALP API unit (AlpSeqAlloc: PicNum, AlpSeqPut: PicLoad)
		long mSequencePictures;
	};

	CAlpSeqSize mAlpSeqSize;
	std::unique_ptr<unsigned char[]> mData;
};

