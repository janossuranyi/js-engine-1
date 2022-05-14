#include "impl/TextureOGL.hpp"
#include "impl/GraphicsDriverOGL.hpp"

namespace jse {

	GLint GetGLPixelInternalFormat(const int aChannel);

	TextureOGL::TextureOGL(GraphicsDriverOGL* aDriver) : Texture()
	{
		mDriver = aDriver;
		mApiId = 0;
	}

	TextureOGL::~TextureOGL()
	{
		if (mApiId)
		{
			glDeleteTextures(1, &mApiId);
		}
	}

	void TextureOGL::Bind()
	{
		if (mApiId)
		{
			GLenum unit = GL_TEXTURE0 + mTextureUnit;
			glActiveTexture(unit);
			glBindTexture(GetGLTextureTypeEnum(mType), mApiId);
		}
	}

	bool TextureOGL::UploadToGPU(const unsigned char* data)
	{
		if (!mApiId)
		{
			glGenTextures(1, &mApiId);
		}

		const GLenum target = GetGLTextureTypeEnum(mType);
		const GLint wrapS = GetGLTextureWrapInt(mWrapS);
		const GLint wrapT = GetGLTextureWrapInt(mWrapT);
		const GLint wrapR = GetGLTextureWrapInt(mWrapR);
		const GLint minFilter = GetGLTextureFilterInt(mMinFilter);
		const GLint magFilter = GetGLTextureFilterInt(mMagFilter);
		const GLint iformat = GetGLPixelInternalFormat(mSize.z);


		glBindTexture(target, mApiId);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);


		if (mType == TextureType_1D)
		{
			glTexImage1D(target, 0, iformat, mSize.x, 0, iformat, GL_UNSIGNED_BYTE, data);
		}
		else if (mType == TextureType_2D)
		{
			glTexImage2D(target, 0, iformat, mSize.x, mSize.y, 0, iformat, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			return false;
		}

		glGenerateMipmap(target);
		glBindTexture(target, 0);

		return true;
	}

	GLint GetGLPixelInternalFormat(const int aChannel)
	{
		switch (aChannel)
		{
			case 1:		return GL_RED;
			case 2:		return GL_RG;
			case 3:		return GL_RGB;
			case 4:		return GL_RGBA;
		}

		return 0;
	}
}