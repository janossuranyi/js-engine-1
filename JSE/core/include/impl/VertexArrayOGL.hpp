#ifndef JSE_VERTEX_ARRAY_OGL_H
#define JSE_VERTEX_ARRAY_OGL_H

#include "graphics/GraphicsTypes.hpp"
#include "graphics/VertexArray.hpp"


namespace jse {
	class GraphicsDriverOGL;

	class VertexArrayOGL : public VertexArray
	{
		friend class GraphicsDriverOGL;

	public:
		VertexArrayOGL(GraphicsDriverOGL* gd, const VertexArrayAttributes& aAttributes, const BufferObject* aIndexBuffer);
		~VertexArrayOGL();
		void Compile();
		void Bind();
	private:
		GraphicsDriverOGL* driver;
		GLuint mVAO;
	};
}
#endif
