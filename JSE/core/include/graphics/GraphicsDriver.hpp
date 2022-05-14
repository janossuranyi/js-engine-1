#ifndef JSE_GRAPHICS_DRIVER_H
#define JSE_GRAPHICS_DRIVER_H

#include "system/SystemTypes.hpp"
#include "graphics/GraphicsTypes.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/VertexArray.hpp"
#include "graphics/GpuShader.hpp"
#include "graphics/Texture.hpp"
/*
=========================================
 Low level graphics API interface
=========================================
*/

namespace jse
{

    //---------------------------------------

    class GraphicsDriver
    {
    public:
        virtual ~GraphicsDriver() {}
        /*
        ===============
        GENERAL SETUP
        ===============
        */
        virtual bool Init(int aWidth, int aHeight, int aDisplay, int aBpp, int aFullscreen, int aMultisampling,
            GpuProgramFormat aGpuProgramFormat, const String& aWindowCaption,
            const Vector2l& aWindowPos, const bool aDebug = false) = 0;

        virtual GpuProgramFormat GetGpuProgramFormat() const = 0;

        /*
        ===============================
        Get graphics card capability
        ===============================
        */

        virtual int GetCaps(GraphicsCaps aType) const = 0;

        virtual bool GetFullScreenEnabled() const = 0;

        virtual void SetVSyncEnabled(bool aEnabled, bool aAdaptiv = false) = 0;

        virtual void SetGammaCorrection(float a0) = 0;

        virtual float GetGammaCorrection() const = 0;

        virtual void SetViewport(const Vector2l& aPos, const Vector2l aSize) = 0;

        /*
        ================================
        Framebuffer operations
        ================================
        */
        virtual void ClearFrameBuffer(ClearFBFlags aClearFlags) = 0;

        virtual void SetClearColor(const Color& aColor) = 0;
        virtual void SetClearDepth(const float aDepth) = 0;
        virtual void SetClearStencil(const int aValue) = 0;
        virtual void FlushCommandBuffers() const = 0;
        virtual void WaitAndFinishCommandBuffers() const = 0;

        virtual void SwapBuffers() const = 0;

        /*
        ==============================
        Render states
        ==============================
        */

        virtual void SetDepthTestEnable(const bool aEnable) = 0;
        virtual void SetDepthTestFunc(const DepthTestFunc aFunc) = 0;
        virtual void SetBlendEnabled(const bool aEnable) = 0;
        virtual void SetBlendFunc(const BlendFunc aSF, const BlendFunc aDF) = 0;
        virtual void SetColorMask(const ColorMask aMask) = 0;
        virtual void SetDepthMask(const bool aFlag) = 0;
        virtual void SetCullFaceEnable(const bool aEnable) = 0;
        virtual void SetFrontFace(const FrontFace aParam) = 0;
        virtual void SetsRGBFrameBufferEnabled(const bool aEnable) = 0;

        virtual bool WaitForExit()= 0;

        //===============================
        //BUFFER operations
        //===============================

        virtual BufferObject* CreateBuffer(const BufferTarget aTarget, const BufferUsage aUsage, const size_t aSize) = 0;

        virtual VertexArray* CreateVertexArray(const BufferObject* aIndexBuffer, const VertexArrayAttributes& aAttributes) = 0;

        virtual GpuShaderStage* CreateGpuShaderStage(const GpuShaderStageType aStage, const String& aName) = 0;
        virtual GpuShader*      CreateGpuShader() = 0;

        virtual Texture* CreateTexture() = 0;
        virtual void SetActiveTexture(const TextureUnit a0) = 0;
        virtual void UnBindBuffer(const BufferTarget aTarget) const = 0;
        virtual void UseShader(GpuShader*) const = 0;

        //=================================
        // Drawing functions
        //=================================


        virtual void DrawPrimitivesWithBase(const PrimitiveType aType, const unsigned int aCount, const size_t aIndices, const unsigned int aBaseVertex) = 0;
    };
}
#endif
