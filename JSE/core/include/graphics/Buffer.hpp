#ifndef JSE_BUFFER_H
#define JSE_BUFFER_H

#include "graphics/GraphicsTypes.hpp"

namespace jse {
	class BufferObject
	{
	public:
        virtual ~BufferObject() {}
		BufferObject(const BufferTarget aTarget, const BufferUsage aUsage,const size_t aSize) : mTarget(aTarget), mSize(aSize), mUsage(aUsage), mIsMapped(false) {}
		virtual bool Alloc(const int size, const void* data = NULL) = 0;
		virtual size_t GetAlloced() const = 0;
		virtual bool UpdateData(const size_t offset, const int size, const void* data) = 0;
		virtual void Bind() const = 0;
		virtual void Destroy() = 0;
		virtual void Reset() = 0;
		virtual void Orphan() = 0;
		virtual void* Map(const BufferAccesType aAccess) = 0;
		virtual void* MapRange(const BufferAccesType aAccess, const size_t offset, const size_t size) = 0;
		virtual void UnMap() = 0;

		virtual void BindToIndex(const int aIndex) = 0;
		virtual void BindToIndexRange(const int aIndex, const unsigned aOffset, const size_t aSize) = 0;

		inline size_t Size() const { return mSize; }
		inline BufferTarget GetBufferTarget() const { return mTarget; }

	protected:
		BufferTarget mTarget;
		BufferUsage mUsage;
		size_t mSize;
		bool mIsMapped;
	};
}

#endif
