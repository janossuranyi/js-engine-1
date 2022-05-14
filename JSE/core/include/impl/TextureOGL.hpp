#ifndef JSE_TEXTURE_OGL_H
#define JSE_TEXTURE_OGL_H

#include "graphics/Texture.hpp"
#include "impl/GraphicsDriverOGL.hpp"
#include <GL/glew.h>

namespace jse {

	class TextureOGL : public Texture
	{
	public:
		TextureOGL(GraphicsDriverOGL* aDriver);
		~TextureOGL();
		void Bind();

	protected:
		bool UploadToGPU(const unsigned char* data);

	private:
		GLuint mApiId;
		GraphicsDriverOGL* mDriver;
	};
}
#endif
