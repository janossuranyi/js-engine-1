
#include <GL/glew.h>
#include "impl/VertexArrayOGL.hpp"
#include "impl/GraphicsDriverOGL.hpp"

namespace jse {

	VertexArrayOGL::VertexArrayOGL(GraphicsDriverOGL* gd, const VertexArrayAttributes& aAttributes, const BufferObject* aIndexBuffer) : VertexArray(aAttributes, aIndexBuffer)
	{
		driver = gd;
		mVAO = 0;
	}

	VertexArrayOGL::~VertexArrayOGL()
	{
		glDeleteVertexArrays(1, &mVAO);
	}

	void VertexArrayOGL::Compile()
	{
		if (mArrayAttributes.GetAttributes().empty())
		{
			return;
		}

		glBindVertexArray(mVAO);
		mIndexBuffer->Bind();

		GLintptr pointer = 0;
		const VertexArrayAttribList list = mArrayAttributes.GetAttributes();
		for (auto it = list.begin(); it != list.end(); it++)
		{
			it->mBuffer->Bind();
			glEnableVertexAttribArray(it->index);
			glVertexAttribPointer(
				it->index,
				it->size,
				GetGLVertexAttribTypeEnum(it->type),
				VertexAttribNormalize[it->type] ? GL_TRUE : GL_FALSE,
				it->stride,
				reinterpret_cast<void*>(pointer + it->basePointer));

			pointer += static_cast<GLintptr>( it->size ) * VertexAttribSizes[it->type];
		}

		glBindVertexArray(0);

		driver->UnBindBuffer(BufferTarget_Vertex);
		driver->UnBindBuffer(BufferTarget_Index);

	}

	void VertexArrayOGL::Bind()
	{
		glBindVertexArray(mVAO);
	}
}
