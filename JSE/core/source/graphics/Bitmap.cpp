#include "graphics/Bitmap.hpp"
#include "system/Heap.hpp"

namespace jse {
	Bitmap::Bitmap() : 
		mNumImages(1),
		mNumMipmaps(1),
		mBytesPerPixel(0),
		mIsCompressed(false),
		mDepth(0),
		mSizeX(0),
		mSizeY(0),
		mPixelFormat(PixelFormat_Unknown)
	{
		mImages.resize(1);
	}

	void Bitmap::Init(const int aNumImages, const int aNumMipmaps)
	{
		mNumImages = aNumImages;
		mNumMipmaps = aNumMipmaps;

		const int size = mNumImages * mNumMipmaps;
		mImages.resize(size);
	}

	BitmapData* Bitmap::GetData(const int aImage, const int aMipLevel)
	{
		if (aImage > mNumImages) return NULL;
		if (aMipLevel > mNumMipmaps) return NULL;

		const int index = aImage * aMipLevel;
		return &mImages[index];
	}

	void Bitmap::SetSize(const int aSizeX, const int aSizeY, const int aDepth)
	{
		mSizeX = aSizeX;
		mSizeY = aSizeY;
		mDepth = aDepth;
	}


	BitmapData::BitmapData() : mSize(0), mData(NULL)
	{
	}

	BitmapData::~BitmapData()
	{
		if (mData)
		{
			delete[] mData;
		}
	}

	void BitmapData::SetData(const unsigned char* aData, const int aSize)
	{
		if (mData && aSize != mSize)
		{
			delete[] mData;
			mData = NULL;
		}

		if (!mData) mData = new unsigned char[aSize];

		mSize = aSize;

		memcpy(mData, aData, aSize);
	}


}