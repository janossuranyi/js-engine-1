#ifndef JSE_TEXTURE_H
#define JSE_TEXTURE_H

#include "system/SystemTypes.hpp"
#include "graphics/GraphicsTypes.hpp"

namespace jse {

	class Texture
	{
	public:
		Texture();
		virtual ~Texture();

		void SetName(const String& aName);
		void SetFileName(const String& aFileName);
		void SetType(const TextureType aType);
		void SetWrapParams(const TextureWrapParam aWrapS, const TextureWrapParam aWrapT, const TextureWrapParam aWrapR = TextureWrapParam_Repeat);
		void SetMinMagFilter(const TextureFilter aMinFilter, const TextureFilter aMagFilter);
		void SetTextureUnit(const TextureUnit a0);

		inline const String& GetName() const { return mName; }
		inline const String& GetFileName() const { return mFileName; }
		inline TextureType GetType() const { return mType; }
		inline TextureWrapParam GetWrapS() const { return mWrapS; }
		inline TextureWrapParam GetWrapT() const { return mWrapT; }
		inline TextureWrapParam GetWrapR() const { return mWrapR; }
		inline TextureUnit GetTextureUnit() const { return mTextureUnit; }

		bool LoadFromFile(const String& aFilename);

		virtual bool UploadToGPU(const unsigned char* data) = 0;
		virtual void Bind() = 0;

	protected:
		String mName;
		String mFileName;
		TextureType mType;
		TextureWrapParam mWrapS, mWrapT, mWrapR;
		TextureFilter mMinFilter, mMagFilter;
		Vector3l mSize;
		TextureUnit mTextureUnit;
		bool mUsable;

	};
}

#endif