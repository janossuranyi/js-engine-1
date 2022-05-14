#ifndef JSE_BITMAP_H
#define JSE_BITMAP_H

#include <vector>

#include "graphics/GraphicsTypes.hpp"

namespace jse {

	class BitmapData
	{
	public:
		BitmapData();
		~BitmapData();

		void SetData(const unsigned char* aData, const int aSize);
	private:
		unsigned char* mData;
		int mSize;
	};

	class Bitmap
	{
	public:
		Bitmap();
		~Bitmap();
		void Init(const int aNumImages, const int aNumMipmaps);
		BitmapData* GetData(const int aImage, const int aMipLevel);
		void SetSize(const int aSizeX, const int aSizeY, const int aDepth);

		inline int GetSizeX() const { return mSizeX; }
		inline int GetSizeY() const { return mSizeY; }
		inline int GetDepth() const { return mDepth; }
		inline PixelFormat GetPixelFormat() const { return mPixelFormat; }
		inline void SetPixelFormat(const PixelFormat a0) { mPixelFormat = a0; }
		inline bool IsCompressed() const { return mIsCompressed; }
		inline void SetCompressed(bool a0) { mIsCompressed = a0; }
		inline int GetBytesPerPixel() const { return mBytesPerPixel; }
		inline void SetBytesPerPixel(const int a0) { mBytesPerPixel = a0; }
		inline int GetNumImages() const { return mNumImages; }
		inline void SetFileName(const String& aFileName) { mFilename = aFileName; }
		inline const String& GetFileName() const { return mFilename;}

	private:
		std::vector<BitmapData> mImages;
		String mFilename;
		bool mIsCompressed;
		int mSizeX, mSizeY, mDepth;
		PixelFormat mPixelFormat;
		int mBytesPerPixel;
		int mNumImages;
		int mNumMipmaps;
	};

}
#endif