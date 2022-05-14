#include "graphics/Texture.hpp"
#include "stb_image.h"

namespace jse {

	Texture::Texture()
	{
		mName = "untitled";
		mFileName = {};
		mType = TextureType_2D;
		mWrapS = TextureWrapParam_Repeat;
		mWrapT = TextureWrapParam_Repeat;
		mWrapR = TextureWrapParam_Repeat;
		mMinFilter = TextureFilter_Linear;
		mMagFilter = TextureFilter_Linear;
		mSize = Vector3l(0);
		mUsable = false;
		mTextureUnit = TextureUnit_0;
	}

	Texture::~Texture() {}

	void Texture::SetName(const String& aName)
	{
		mName = aName;
	}

	void Texture::SetFileName(const String& aFileName)
	{
		mFileName = aFileName;
	}

	void Texture::SetType(const TextureType aType)
	{
		mType = aType;
	}

	void Texture::SetWrapParams(const TextureWrapParam aWrapS, const TextureWrapParam aWrapT, const TextureWrapParam aWrapR)
	{
		mWrapS = aWrapS;
		mWrapT = aWrapT;
		mWrapR = aWrapR;
	}

	void Texture::SetMinMagFilter(const TextureFilter aMinFilter, const TextureFilter aMagFilter)
	{
		mMinFilter = aMinFilter;
		mMagFilter = aMagFilter;

	}

	void Texture::SetTextureUnit(const TextureUnit a0)
	{
		mTextureUnit = a0;
	}

	bool Texture::LoadFromFile(const String& aFilename)
	{
		int x, y, n;
		bool result = false;

		mFileName = aFilename;

		stbi_set_flip_vertically_on_load(true);

		unsigned char* data = stbi_load(mFileName.c_str(), &x, &y, &n, 0);

		if (data)
		{
			mSize.x = x;
			mSize.y = y;
			mSize.z = n;

			result = UploadToGPU(data);
			stbi_image_free(data);
		}

		return result;;
	}

}
