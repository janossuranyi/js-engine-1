#include "impl/GraphicsDriverOGL.hpp"
#include "impl/BufferOGL.hpp"
#include "impl/VertexArrayOGL.hpp"
#include "impl/GpuShaderOGL.hpp"
#include "impl/TextureOGL.hpp"

namespace jse {


	//typedef void (GLAPIENTRY *GLDEBUGPROCARB)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

	void GLAPIENTRY OGLDebugOutputCallback(GLenum alSource, GLenum alType, GLuint alID, GLenum alSeverity, GLsizei alLength, const GLchar* apMessage, const void* apUserParam)
	{
		Info("Source: %d Type: %d Id: %d Severity: %d '%s'", alSource, alType, alID, alSeverity, apMessage);
	}

    GLenum GetGLFrontFaceEnum(const FrontFace aFace)
    {
        switch(aFace)
        {
            case FrontFace_CW:          return GL_CW;
            case FrontFace_CCW:         return GL_CCW;
            default:
                return GL_CCW;
        }
    }

	GLenum GetGLCullFaceEnum(const CullFace aFace)
	{
		switch (aFace)
		{
			case CullFace_Front:		return GL_FRONT;
			case CullFace_Back:			return GL_BACK;
			case CullFace_FrontAndBack:	return GL_FRONT_AND_BACK;
			default:
				return GL_FRONT;
		}
	}

	GLenum GetGLShaderStageTypeEnum(const GpuShaderStageType aStage)
	{
		switch (aStage)
		{
			case ShaderStage_Vertex:		return GL_VERTEX_SHADER;
			case ShaderStage_Fragment:		return GL_FRAGMENT_SHADER;
			case ShaderStage_Compute:		return GL_COMPUTE_SHADER;
			case ShaderStage_Geometry:		return GL_GEOMETRY_SHADER;
			case ShaderStage_Tesselation:	return GL_TESS_CONTROL_SHADER;
			default:
				return 0;
		}
	}

	GLenum GetGLBufferAccessTypeEnum(const BufferAccesType aType)
	{
		switch (aType)
		{
			case BufferAccess_Readonly:		return GL_READ_ONLY;
			case BufferAccess_ReadWrite:	return GL_READ_WRITE;
			case BufferAccess_WriteOnly:	return GL_WRITE_ONLY;
			default:
				return 0;
		}

	}

	GLenum GetGLTextureTypeEnum(const TextureType aType)
	{
		switch (aType)
		{
			case TextureType_1D:		return GL_TEXTURE_1D;
			case TextureType_2D:		return GL_TEXTURE_2D;
			case TextureType_3D:		return GL_TEXTURE_3D;
			case TextureType_CubeMap:	return GL_TEXTURE_CUBE_MAP;
			default:
				return 0;
		}
	}

	GLenum GetGLTextureParamEnum(const TextureParam aParam)
	{
		switch (aParam)
		{
			case TextureParam_MagFilter:		return GL_TEXTURE_MAG_FILTER;
			case TextureParam_MinFilter:		return GL_TEXTURE_MIN_FILTER;
			case TextureParam_Wrap_S:			return GL_TEXTURE_WRAP_S;
			case TextureParam_Wrap_T:			return GL_TEXTURE_WRAP_T;
			case TextureParam_Wrap_R:			return GL_TEXTURE_WRAP_R;
			default:
				return 0;
		}
	}

	GLint GetGLTextureWrapInt(const TextureWrapParam aParam)
	{
		switch (aParam)
		{
			case TextureWrapParam_ClampToEdge:			return GL_CLAMP_TO_EDGE;
			case TextureWrapParam_Repeat:				return GL_REPEAT;
			case TextureWrapParam_ClampToBorder:		return GL_CLAMP_TO_BORDER;
			case TextureWrapParam_MirrorClampToEdge:	return GL_MIRROR_CLAMP_TO_EDGE;
			case TextureWrapParam_MirroredRepeat:		return GL_MIRRORED_REPEAT;
			default:
				return 0;
		}
	}

	GLint GetGLTextureFilterInt(const TextureFilter aParam)
	{
		switch (aParam)
		{
			case TextureFilter_Linear:					return GL_LINEAR;
			case TextureFilter_Nearest:					return GL_NEAREST;
			case TextureFilter_Nearest_Mipmap_Nearest:	return GL_NEAREST_MIPMAP_NEAREST;
			case TextureFilter_Nearest_Mipmap_Linear:	return GL_NEAREST_MIPMAP_LINEAR;
			case TextureFilter_Linear_Mipmap_Linear:	return GL_LINEAR_MIPMAP_LINEAR;
			case TextureFilter_Linear_Mipmap_Nearest:	return GL_LINEAR_MIPMAP_NEAREST;
			default:
				return 0;
		}
	}

	GLenum GetGLPrimitiveEnum(const PrimitiveType aParam)
	{
		switch (aParam)
		{
			case PrimitiveType_LineLoop:		return GL_LINE_LOOP;
			case PrimitiveType_LineStrip:		return GL_LINE_STRIP;
			case PrimitiveType_Lines:			return GL_LINES;
			case PrimitiveType_Points:			return GL_POINTS;
			case PrimitiveType_TriangleStrip:	return GL_TRIANGLE_STRIP;
			case PrimitiveType_TriangleFan:		return GL_TRIANGLE_FAN;
			case PrimitiveType_Triangles:		return GL_TRIANGLES;
			default:
				return 0;
		}
	}

	GLenum GetGLVertexAttribTypeEnum(const VertexAttribType aType)
	{
		switch (aType)
		{
			case VtxAttribType_Float:		return GL_FLOAT;
			case VtxAttribType_UByte:		return GL_UNSIGNED_BYTE;
			case VtxAttribType_UInt:		return GL_UNSIGNED_INT;
			default:
				return 0;
		}
	}

	GLenum GetGLBufferUsageEnum(const BufferUsage aUsage)
	{
		switch (aUsage)
		{
			case BufferUsage_StaticDraw:		return GL_STATIC_DRAW;
			case BufferUsage_StaticRead:		return GL_STATIC_READ;
			case BufferUsage_StaticCopy:		return GL_STATIC_COPY;
			case BufferUsage_StreamDraw:		return GL_STREAM_DRAW;
			case BufferUsage_StreamRead:		return GL_STREAM_READ;
			case BufferUsage_StreamCopy:		return GL_STREAM_COPY;
			case BufferUsage_DynaDraw:			return GL_DYNAMIC_DRAW;
			case BufferUsage_DynaRead:			return GL_DYNAMIC_READ;
			case BufferUsage_DynaCopy:			return GL_DYNAMIC_COPY;
			default:
				return 0;
		}
	}

	GLenum GetGLBufferTargetEnum(const BufferTarget aTarget)
	{
		switch (aTarget)
		{
			case BufferTarget_Vertex:		return GL_ARRAY_BUFFER;
			case BufferTarget_Index:		return GL_ELEMENT_ARRAY_BUFFER;
			case BufferTarget_Uniform:		return GL_UNIFORM_BUFFER;
			case BufferTarget_Query:		return GL_QUERY_BUFFER;
			default:
				return 0;
		}
	}

	GLenum GetGLDepthFuncEnum(const DepthTestFunc aType)
	{
		switch (aType)
		{
			case DepthTestFunc_Always:          return GL_ALWAYS;
			case DepthTestFunc_Equal:           return GL_EQUAL;
            case DepthTestFunc_LessOrEqual:     return GL_LEQUAL;
			case DepthTestFunc_Greater:         return GL_GREATER;
			case DepthTestFunc_GreaterOrEqual:  return GL_GEQUAL;
			case DepthTestFunc_Less:            return GL_LESS;
			case DepthTestFunc_NotEqual:        return GL_NOTEQUAL;
			case DepthTestFunc_Never:           return GL_NEVER;
			default:
				return 0;
		}
	}

	GLenum GetGLBlendFuncEnum(const BlendFunc aType)
	{
		switch (aType)
		{
			case BlendFunc_DestAlpha:           return GL_DST_ALPHA;
			case BlendFunc_DestColor:           return GL_DST_COLOR;
			case BlendFunc_One:                 return GL_ONE;
			case BlendFunc_OneMinusDestAlpha:   return GL_ONE_MINUS_DST_ALPHA;
			case BlendFunc_OneMinusDestColor:   return GL_ONE_MINUS_DST_COLOR;
			case BlendFunc_OneMinusSrcAlpha:    return GL_ONE_MINUS_SRC_ALPHA;
			case BlendFunc_OneMinusSrcColor:    return GL_ONE_MINUS_SRC_COLOR;
			case BlendFunc_SrcAlpha:            return GL_SRC_ALPHA;
			case BlendFunc_SrcAlphaSaturate:    return GL_SRC_ALPHA_SATURATE;
			case BlendFunc_SrcColor:            return GL_SRC_COLOR;
			case BlendFunc_Zero:                return GL_ZERO;
			default:
				return 0;
		}
	}

	GraphicsDriverOGL::GraphicsDriverOGL()
	{
		initHasBeenRun = false;
		isFullscreen = false;
		gammaCorrection = 1.0f;
		gpuProgramFormat = GpuProgramFormat_LastEnum;

		gl_Context = 0;
		pWindow = 0;
		vMajor = vMinor = 0;
	}

	GraphicsDriverOGL::~GraphicsDriverOGL()
	{
		if (gl_Context)
		{
			SDL_GL_DeleteContext(gl_Context);
		}

		if (pWindow)
		{
			SDL_DestroyWindow(pWindow);
		}
	}

	void GraphicsDriverOGL::SetDepthTestEnable(const bool aEnable)
	{
		if (noStateCache || aEnable != stDepthTestEnabled)
		{
			stDepthTestEnabled = aEnable;
			if (aEnable) glEnable(GL_DEPTH_TEST);
			else glDisable(GL_DEPTH_TEST);
		}
	}

	void GraphicsDriverOGL::SetDepthTestFunc(const DepthTestFunc aFunc)
	{
		if (noStateCache || stDepthFunc != aFunc)
		{
			stDepthFunc = aFunc;
			glDepthFunc(GetGLDepthFuncEnum(aFunc));
		}
	}

	void GraphicsDriverOGL::SetBlendFunc(const BlendFunc aSF, const BlendFunc aDF)
	{
		glBlendFunc(GetGLBlendFuncEnum(aSF), GetGLBlendFuncEnum(aDF));
	}

	void GraphicsDriverOGL::SetsRGBFrameBufferEnabled(const bool aEnable)
	{
		if (aEnable)
		{
			glEnable(GL_FRAMEBUFFER_SRGB);
		}
		else
		{
			glDisable(GL_FRAMEBUFFER_SRGB);
		}
	}

	void GraphicsDriverOGL::SetColorMask(const ColorMask aMask)
	{
		glColorMask(
			(aMask & ColorMask_Red) != 0,
			(aMask & ColorMask_Green) != 0,
			(aMask & ColorMask_Blue) != 0,
			(aMask & ColorMask_Alpha) != 0
		);
	}

	void GraphicsDriverOGL::SetDepthMask(const bool aFlag)
	{
		glDepthMask(aFlag);
	}

	void GraphicsDriverOGL::SetCullFaceEnable(const bool aEnable)
	{
        if (aEnable)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
	}

	void GraphicsDriverOGL::SetFrontFace(const FrontFace aParam)
	{
		glFrontFace(GetGLFrontFaceEnum(aParam));
	}

	bool GraphicsDriverOGL::WaitForExit()
	{
		SDL_Event e;
		bool quit = false;

		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					quit = true;
					break;
				}
			}
			else if (e.type == SDL_WINDOWEVENT)
			{
				switch (e.window.event)
				{
				case SDL_WINDOWEVENT_RESIZED:
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					Info("Window %d resized to %dx%d",
						e.window.windowID, e.window.data1,
						e.window.data2);
					//framebuffer_size_callback(e.window.data1, e.window.data2);
					const Vector2l origin(0, 0);
					const Vector2l size(e.window.data1, e.window.data2);

					SetViewport(origin, size);

					break;
				}

			}
		}

		return quit;
	}

	void GraphicsDriverOGL::UseShader(GpuShader* aShader) const
	{
		if (aShader == nullptr)
		{
			glUseProgram(0);
		}
		else
		{
			aShader->Use();
		}
	}

	void GraphicsDriverOGL::SetSdlGlAttributes(const int aMultisamples, const bool aDebug)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, SDL_TRUE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);

		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);

		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, aMultisamples > 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, aMultisamples);

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OGL_MAJOR_MIN_VERSION);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OGL_MINOR_MIN_VERSION);
		SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, SDL_TRUE);


		if (aDebug)
		{
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
		}
	}

	bool GraphicsDriverOGL::Init(int aWidth, int aHeight, int aDisplay, int aBpp, int aFullscreen, int aMultisampling,
		GpuProgramFormat aGpuProgramFormat, const String& aWindowCaption,
		const Vector2l& aWindowPos, const bool aDebug)
	{
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
		{
			Error("SDL_Init failed: %s", SDL_GetError());
			return false;
		}

		SetSdlGlAttributes(aMultisampling, true);
		Uint32 lWindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
		if (aFullscreen
			&& !aHeight
			&& !aWidth)
		{
			lWindowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		else if (aFullscreen) {
			lWindowFlags |= SDL_WINDOW_FULLSCREEN;
		}
		else
		{
			lWindowFlags |= SDL_WINDOW_RESIZABLE;
		}

		//window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w_width, w_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
		pWindow = SDL_CreateWindow(
			aWindowCaption.c_str(),
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			aWidth,
			aHeight,
			lWindowFlags);

		if (!pWindow)
		{
			Error("Window could not be created! SDL Error: %s", SDL_GetError());
			return false;
		}

		gl_Context = SDL_GL_CreateContext(pWindow);

		if (gl_Context == 0)
		{
			Error("Cannot create OpenGL context (%d.%d) SDL Error: %s", OGL_MAJOR_MIN_VERSION, OGL_MINOR_MIN_VERSION, SDL_GetError());
			return false;
		}

		SDL_GL_MakeCurrent(pWindow, gl_Context);

		// Initialize GLEW
		glewExperimental = true; // Needed for core profile
		if (glewInit() != GLEW_OK) {
			Error("Failed to initialize GLEW");
			return false;
		}

		isFullscreen = aFullscreen;
		gpuProgramFormat = aGpuProgramFormat;

		initHasBeenRun = true;

		{
			int r, g, b, a, d, s;
			SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &r);
			SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &g);
			SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &b);
			SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &a);
			SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &d);
			SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &s);


			GLint ma = 0, mi = 0;
			glGetIntegerv(GL_MAJOR_VERSION, &ma);
			glGetIntegerv(GL_MINOR_VERSION, &mi);

			const int gl_version = 100 * ma + 10 * mi;

			if (gl_version < OGL_EXPECTED_VERSION)
			{
				Error("OpenGL context must be at leat %d.%d!", OGL_MAJOR_MIN_VERSION, OGL_MINOR_MIN_VERSION);
				SDL_GL_DeleteContext(gl_Context);
				gl_Context = 0;

				return false;
			}

			if (GLEW_ARB_debug_output && aDebug)
			{
				glDebugMessageCallbackARB(&OGLDebugOutputCallback, NULL);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
			}
			else
			{
				Warning("OGL debug output not supported!");
			}


			Info("Got %d stencil bits, %d depth bits, color bits: r%d g%d b%d a%d", s, d, r, g, b, a);

			contextInfo.majorVer = ma;
			contextInfo.minorVer = mi;
			contextInfo.alphabits = a;
			contextInfo.redbits = r;
			contextInfo.greenbits = g;
			contextInfo.bluebits = b;
			contextInfo.depthbits = d;
			contextInfo.stencilbits = s;
		}

		GLint n = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &n);

		GLubyte const* gl_renderer = glGetString(GL_RENDERER);
		GLubyte const* gl_version = glGetString(GL_VERSION);

		Info("GL Renderer: %s", gl_renderer);
		Info("GL Version: %s", gl_version);
		Info("GL Extensions: %d", n);

		//SDL_SetRelativeMouseMode(SDL_TRUE);
		//SDL_CaptureMouse(SDL_TRUE);

		return true;
	}

	void GraphicsDriverOGL::SwapBuffers() const
	{
		SDL_GL_SwapWindow(pWindow);
	}

	void GraphicsDriverOGL::SetBlendEnabled(const bool aEnabled)
	{
		if (noStateCache || stBlendEnabled != aEnabled)
		{
			stBlendEnabled = aEnabled;
			if (aEnabled) glEnable(GL_BLEND);
			else glDisable(GL_BLEND);
		}
	}

	int GraphicsDriverOGL::GetCaps(GraphicsCaps aType) const
	{
		GLint n = 0;

		switch (aType)
		{
			case GraphicsCaps_MaxTextureImageUnits:		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &n); break;
			case GraphicsCaps_TextureCompression:		return GLEW_ARB_texture_compression ? 1 : 0;
			case GraphicCaps_ShaderModel_4:				return GLEW_EXT_gpu_shader4 ? 1 : 0;
			case GraphicCaps_ShaderModel_5:				return GLEW_EXT_gpu_shader5 ? 1 : 0;
			case GraphicCaps_AutoGenerateMipMaps:		return GLEW_SGIS_generate_mipmap ? 1 : 0;
			case GraphicCaps_TextureCompression_DXTC:	return GLEW_EXT_texture_compression_s3tc ? 1 : 0;
			case GraphicCaps_RenderToTexture:			return GLEW_EXT_framebuffer_object ? 1 : 0;
			case GraphicCaps_MaxAnisotropicFiltering:
			{
				if (!GLEW_EXT_texture_filter_anisotropic) return 0;

				float Max;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &Max);
				return (int)Max;
			}

		}

		return n;
	}

	void GraphicsDriverOGL::SetVSyncEnabled(int aEnabled, bool aAdaptiv)
	{
		SDL_GL_SetSwapInterval(aEnabled && aAdaptiv ? -1 : aEnabled);
	}

	void GraphicsDriverOGL::SetGammaCorrection(float a0)
	{
		gammaCorrection = a0;
		SDL_SetWindowBrightness(pWindow, a0);
	}

	void GraphicsDriverOGL::ClearFrameBuffer(const ClearFBFlags aFlags)
	{
		GLbitfield flags = 0;

		if (aFlags & ClearFBFlags_Color)	flags |= GL_COLOR_BUFFER_BIT;
		if (aFlags & ClearFBFlags_Depth)	flags |= GL_DEPTH_BUFFER_BIT;
		if (aFlags & ClearFBFlags_Stencil)	flags |= GL_STENCIL_BUFFER_BIT;

		glClear(flags);
	}

	void GraphicsDriverOGL::SetViewport(const Vector2l& aPos, const Vector2l aSize)
	{
		glViewport(aPos.x, aPos.y, aSize.x, aSize.y);
	}

	void GraphicsDriverOGL::SetClearColor(const Color& aColor)
	{
		glClearColor(aColor.r, aColor.g, aColor.b, aColor.a);
	}

	void GraphicsDriverOGL::SetClearDepth(const float aDepth)
	{
		glClearDepth(aDepth);
	}

	void GraphicsDriverOGL::SetClearStencil(const int aValue)
	{
		glClearStencil(aValue);
	}

	void GraphicsDriverOGL::FlushCommandBuffers() const
	{
		glFlush();
	}

	void GraphicsDriverOGL::WaitAndFinishCommandBuffers() const
	{
		glFinish();
	}

	BufferObject* GraphicsDriverOGL::CreateBuffer(const BufferTarget aTarget, const BufferUsage aUsage, const size_t aSize)
	{
		BufferObjectOGL* buf = new BufferObjectOGL(this, aTarget, aUsage, aSize);

		GLuint apiId;
		GLenum target = GetGLBufferTargetEnum(aTarget);
		GLenum usage = GetGLBufferUsageEnum(aUsage);

		glGenBuffers(1, &apiId);
		glBindBuffer(target, apiId);
		glBufferData(target, aSize, NULL, usage);
		glBindBuffer(target, 0);

		buf->mApiId = apiId;

		return buf;
	}

	VertexArray* GraphicsDriverOGL::CreateVertexArray(const BufferObject* aIndexBuffer, const VertexArrayAttributes& aAttributes)
	{
		VertexArrayOGL* va = new VertexArrayOGL(this, aAttributes, aIndexBuffer);
		GLuint vao;
		glGenVertexArrays(1, &vao);

		va->mVAO = vao;

		return va;
	}

	GpuShaderStage* GraphicsDriverOGL::CreateGpuShaderStage(const GpuShaderStageType aStage, const String& aName)
	{
		GpuShaderStageOGL* stage = new GpuShaderStageOGL(aStage, aName);

		return stage;
	}

	GpuShader* GraphicsDriverOGL::CreateGpuShader()
	{
		return new GpuShaderOGL();
	}
	Texture* GraphicsDriverOGL::CreateTexture()
	{
		return new TextureOGL(this);
	}
	void GraphicsDriverOGL::SetActiveTexture(const TextureUnit a0)
	{
		const GLenum unit = GL_TEXTURE0 + a0;

		glActiveTexture(unit);
	}

	void GraphicsDriverOGL::DrawPrimitivesWithBase(const PrimitiveType aType, const unsigned int aCount, const size_t aIndices, const unsigned int aBaseVertex)
	{
		const GLenum type = GetGLPrimitiveEnum(aType);
		glDrawElementsBaseVertex(type, aCount, GL_UNSIGNED_SHORT, reinterpret_cast<void*>(aIndices), aBaseVertex);
	}
}
