#include "system/Logger.hpp"
#include "impl/BufferOGL.hpp"
#include "impl/GraphicsDriverOGL.hpp"

namespace jse {

	void CopyBuffer(u8* dst, const u8* src, int numBytes);

	BufferObjectOGL::BufferObjectOGL(GraphicsDriverOGL* gd, BufferTarget aTarget, const BufferUsage aUsage,const size_t aSize) : BufferObject(aTarget, aUsage, aSize)
	{
		mDriver = gd;
		mApiId = 0;
		mAlloced = 0;
		mTargetEnum = GetGLBufferTargetEnum(aTarget);
		mPtr = nullptr;
	}

	BufferObjectOGL::~BufferObjectOGL()
	{
		Destroy();
	}

	bool BufferObjectOGL::Alloc(const int aSize, const void* data)
	{

		if (!mIsMapped) {
			if (data == NULL || UpdateData(mAlloced, aSize, data))
			{
				mAlloced += aSize;
				return true;
			}
		}

		return false;
	}

	bool BufferObjectOGL::UpdateData(const size_t aOffset, const int aSize, const void* data)
	{
		//if (mIsMapped) return false;
		assert(((u64)data & 15) == 0);

		if (aOffset + aSize > mSize)
		{
			Warning("BufferObjectOGL.%d: Buffer update failed, source exceeds buffer size!", __LINE__);
			return false;
		}

		if (data == NULL) {
			Warning("BufferObjectOGL.%d: Buffer update failed, data is null", __LINE__);
			return false;
		}

		if (mIsMapped)
		{
			CopyBuffer((u8*)mPtr, (u8*)data, aSize);
		}
		else
		{
			glBufferSubData(mTargetEnum, aOffset, aSize, data);
		}

		return true;
	}

	void BufferObjectOGL::Init(const GLuint aApiId)
	{
		mApiId = aApiId;
	}

	void BufferObjectOGL::Bind() const
	{
		glBindBuffer(mTargetEnum, mApiId);
	}

	void BufferObjectOGL::Reset()
	{
		mAlloced = 0;
	}

	void BufferObjectOGL::Destroy()
	{
		if (mIsMapped)
		{
			Bind();
			UnMap();
		}

		mDriver->UnBindBuffer(mTarget);
		glDeleteBuffers(1, &mApiId);

		mSize = 0;
		mAlloced = 0;
		mApiId = 0;
	}

	void BufferObjectOGL::Orphan()
	{
		glBufferData(mTargetEnum, mSize, NULL, GetGLBufferUsageEnum(mUsage));
	}

	void* BufferObjectOGL::Map(const BufferAccesType aAccess)
	{
		if (!mIsMapped)
		{
			GLbitfield xAccess = GetGLBufferRangeAccessBit(aAccess);

			if (xAccess & GL_MAP_WRITE_BIT)
			{
				xAccess |= /*GL_MAP_INVALIDATE_BUFFER_BIT |*/ GL_MAP_UNSYNCHRONIZED_BIT;
			}

			void* ptr = glMapBufferRange(mTargetEnum, 0, mSize, xAccess);
			if (ptr != NULL)
			{
				mIsMapped = true;
				mPtr = ptr;
			}

			return ptr;
		}

		return NULL;
	}

	void* BufferObjectOGL::MapRange(const BufferAccesType aAccess, const size_t offset, const size_t size)
	{
		return NULL;
	}

	void BufferObjectOGL::UnMap()
	{
		if (mIsMapped)
		{
			glBindBuffer(mTargetEnum, mApiId);
			glUnmapBuffer(mTargetEnum);
			glBindBuffer(mTargetEnum, 0);
			mPtr = nullptr;
			mIsMapped = false;
		}
	}

	void BufferObjectOGL::BindToIndex(const int aIndex)
	{
		if (!mIsMapped && mTarget == BufferTarget_Uniform)
		{
			glBindBufferBase(mTargetEnum, aIndex, mApiId);
		}
	}

	void BufferObjectOGL::BindToIndexRange(const int aIndex, const unsigned aOffset, const size_t aSize)
	{
		if (!mIsMapped && mTarget == BufferTarget_Uniform)
		{
			glBindBufferRange(mTargetEnum, aIndex, mApiId, aOffset, aSize);
		}
	}
	GLbitfield GetGLBufferRangeAccessBit(const BufferAccesType aType)
	{
		switch (aType)
		{
			case BufferAccess_Readonly:		return GL_MAP_READ_BIT;
			case BufferAccess_ReadWrite:	return GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
			case BufferAccess_WriteOnly:	return GL_MAP_WRITE_BIT;
		}

		return 0;
	}

#if defined(USE_INTRINSICS_SSE)

	void CopyBuffer(u8* dst, const u8* src, int numBytes)
	{

		int i = 0;
		for (; i + 128 <= numBytes; i += 128)
		{
			__m128i d0 = _mm_load_si128((__m128i*) & src[i + 0 * 16]);
			__m128i d1 = _mm_load_si128((__m128i*) & src[i + 1 * 16]);
			__m128i d2 = _mm_load_si128((__m128i*) & src[i + 2 * 16]);
			__m128i d3 = _mm_load_si128((__m128i*) & src[i + 3 * 16]);
			__m128i d4 = _mm_load_si128((__m128i*) & src[i + 4 * 16]);
			__m128i d5 = _mm_load_si128((__m128i*) & src[i + 5 * 16]);
			__m128i d6 = _mm_load_si128((__m128i*) & src[i + 6 * 16]);
			__m128i d7 = _mm_load_si128((__m128i*) & src[i + 7 * 16]);
			_mm_stream_si128((__m128i*) & dst[i + 0 * 16], d0);
			_mm_stream_si128((__m128i*) & dst[i + 1 * 16], d1);
			_mm_stream_si128((__m128i*) & dst[i + 2 * 16], d2);
			_mm_stream_si128((__m128i*) & dst[i + 3 * 16], d3);
			_mm_stream_si128((__m128i*) & dst[i + 4 * 16], d4);
			_mm_stream_si128((__m128i*) & dst[i + 5 * 16], d5);
			_mm_stream_si128((__m128i*) & dst[i + 6 * 16], d6);
			_mm_stream_si128((__m128i*) & dst[i + 7 * 16], d7);
		}
		for (; i + 16 <= numBytes; i += 16)
		{
			__m128i d = _mm_load_si128((__m128i*) & src[i]);
			_mm_stream_si128((__m128i*) & dst[i], d);
		}
		for (; i + 4 <= numBytes; i += 4)
		{
			*(u32*)&dst[i] = *(const u32*)&src[i];
		}
		for (; i < numBytes; i++)
		{
			dst[i] = src[i];
		}
		_mm_sfence();
	}

#else

	void CopyBuffer(u8* dst, const u8* src, int numBytes)
	{
		memcpy(dst, src, numBytes);
	}

#endif

}
