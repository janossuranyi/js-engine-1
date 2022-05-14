#ifndef JSE_BUFFER_OGL_H
#define JSE_BUFFER_OGL_H

#include <GL/glew.h>
#include "graphics/Buffer.hpp"
#include "impl/GraphicsDriverOGL.hpp"

namespace jse {

	GLbitfield GetGLBufferRangeAccessBit(const BufferAccesType aType);


	class BufferObjectOGL : public BufferObject
	{
		friend class GraphicsDriverOGL;

	public:
		BufferObjectOGL(GraphicsDriverOGL* gd, const BufferTarget aTarget, const BufferUsage aUsage ,const size_t aSize);
		~BufferObjectOGL();

		bool Alloc(const int size, const void* data = NULL);
		size_t GetAlloced() const { return mAlloced; }
		bool UpdateData(const size_t offset, const int size, const void* data);
		void Bind() const;
		void Reset();
		void Destroy();
		void Orphan();
		void* Map(const BufferAccesType aAccess);
		void* MapRange(const BufferAccesType aAccess, const size_t offset, const size_t size);
		void UnMap();

		void BindToIndex(const int aIndex);
		void BindToIndexRange(const int aIndex, const unsigned aOffset, const size_t aSize);


	private:
		BufferObjectOGL() = delete;
		void Init(const GLuint aApiId);

		size_t mAlloced;
		GLuint mApiId;
		GLenum mTargetEnum;
		GraphicsDriverOGL* mDriver;
		void* mPtr;
	};
}

#endif
