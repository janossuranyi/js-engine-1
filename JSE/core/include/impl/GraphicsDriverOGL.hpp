#ifndef JSE_GRAPHICS_DRIVER_OGL_H
#define JSE_GRAPHICS_DRIVER_OGL_H

#include <GL/glew.h>
#include <SDL.h>
#include "system/Logger.hpp"
#include "graphics/GraphicsDriver.hpp"

namespace jse {

    /*
    ======================
    GLOBAL FUNCTIONS
    ======================
    */

    GLenum GetGLDepthFuncEnum(const DepthTestFunc aType);
    GLenum GetGLBlendFuncEnum(const BlendFunc aType);
    GLenum GetGLBufferTargetEnum(const BufferTarget aTarget);
    GLenum GetGLBufferUsageEnum(const BufferUsage aUsage);
    GLenum GetGLVertexAttribTypeEnum(const VertexAttribType aType);
    GLenum GetGLShaderStageTypeEnum(const GpuShaderStageType aStage);
    GLenum GetGLBufferAccessTypeEnum(const BufferAccesType aType);
    GLenum GetGLTextureTypeEnum(const TextureType aType);
    GLenum GetGLTextureParamEnum(const TextureParam aParam);
    GLint GetGLTextureWrapInt(const TextureWrapParam aParam);
    GLint GetGLTextureFilterInt(const TextureFilter aParam);
    GLenum GetGLPrimitiveEnum(const PrimitiveType aParam);

    struct GLConfig {
        int majorVer;
        int minorVer;
        int redbits;
        int greenbits;
        int bluebits;
        int alphabits;
        int depthbits;
        int stencilbits;
    };

    //-------------------------------------------------------


    class GraphicsDriverOGL : public GraphicsDriver
    {
    public:

        const int OGL_MAJOR_MIN_VERSION = 4;
        const int OGL_MINOR_MIN_VERSION = 1;
        const int OGL_EXPECTED_VERSION = 100 * OGL_MAJOR_MIN_VERSION + 10 * OGL_MINOR_MIN_VERSION;

        GraphicsDriverOGL();
        ~GraphicsDriverOGL();
        bool Init(int aWidth, int aHeight, int aDisplay, int aBpp, int aFullscreen, int aMultisampling,
            GpuProgramFormat aGpuProgramFormat, const String& aWindowCaption,
            const Vector2l& aWindowPos, const bool aDebug = false);

        void SwapBuffers() const;
        void SetBlendEnabled(const bool aEnabled);
        int GetCaps(GraphicsCaps aType) const;
        void SetVSyncEnabled(int aEnabled, bool aAdaptiv = false);
        void SetGammaCorrection(float a0);
        void ClearFrameBuffer(const ClearFBFlags aFlags);
        void SetViewport(const Vector2l& aPos, const Vector2l aSize);
        void SetClearColor(const Color& aColor);
        void SetClearDepth(const float aDepth);
        void SetClearStencil(const int aValue);

        void FlushCommandBuffers() const;
        void WaitAndFinishCommandBuffers() const;

        inline float GetGammaCorrection() const { return gammaCorrection; }
        inline bool GetFullScreenEnabled() const { return isFullscreen; }

        void SetDepthTestEnable(const bool aEnable);
        void SetDepthTestFunc(const DepthTestFunc aFunc);
        void SetBlendFunc(const BlendFunc aSF, const BlendFunc aDF);
        void SetsRGBFrameBufferEnabled(const bool aEnable);
        void SetColorMask(const ColorMask aMask);
        void SetDepthMask(const bool aFlag);
        void SetCullFaceEnable(const bool aEnable);
        void SetFrontFace(const FrontFace aParam);

        inline GpuProgramFormat GetGpuProgramFormat() const { return gpuProgramFormat; }

        bool WaitForExit();

        BufferObject* CreateBuffer(const BufferTarget aTarget, const BufferUsage aUsage, const size_t aSize);

        VertexArray* CreateVertexArray(const BufferObject* aBuffer, const VertexArrayAttributes& aAttributes);

        GpuShaderStage* CreateGpuShaderStage(const GpuShaderStageType aStage, const String& aName);

        GpuShader* CreateGpuShader();

        Texture* CreateTexture();

        void SetActiveTexture(const TextureUnit a0);

        void DrawPrimitivesWithBase(const PrimitiveType aType, const unsigned int aCount, const size_t aIndices, const unsigned int aBaseVertex);

        /*
        ============================
        Low Level helper functions
        ============================
        */
        
        inline void UnBindBuffer(const BufferTarget aTarget) const { glBindBuffer(GetGLBufferTargetEnum( aTarget ), 0); }
        void UseShader(GpuShader*) const;

    private:
        void SetSdlGlAttributes(const int aMultisamples, const bool aDebug = false);
    private:
        bool initHasBeenRun;
        SDL_Window* pWindow;
        SDL_GLContext gl_Context;
        GLint vMajor, vMinor;
        bool noStateCache;
        bool isFullscreen;
        float gammaCorrection;
        GpuProgramFormat gpuProgramFormat;
        GLConfig contextInfo{};

        /*
        * ===========================
        * STATE CACHE VARS
        * ===========================
        */

    };
}

#endif
