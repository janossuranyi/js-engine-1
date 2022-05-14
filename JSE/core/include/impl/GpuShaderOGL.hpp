#ifndef JSE_GPU_SHADER_OGL_H
#define JSE_GPU_SHADER_OGL_H
#include <unordered_map>
#include <GL/glew.h>
#include "graphics/GpuShader.hpp"

namespace jse {

    class GpuShaderStageOGL;

    typedef std::unordered_map<String, GLint> UniformCacheType;

    class GpuShaderOGL : public GpuShader
    {
    public:
        GpuShaderOGL() : GpuShader(), mProgramId(0) {}
        ~GpuShaderOGL();
        void AddStage(GpuShaderStage* aShader);
        void AddStage(const GpuShaderStageType aStage, const String& aSourceName, const String& aName);
        bool Compile();
        void Delete();
        void Use();

        void SetFloat(const String& aName, const float aVal);
        void SetFloatv(const String& aName, const size_t aSize, const float* aVal);
        void SetInt(const String& aName, const int aVal);
        void SetVector3(const String&, const float*);
        void SetVector4(const String&, const float*);
        void SetMatrix(const String&, const float*);
        void SetMatrix3(const String&, const float*);
        void BindUniformBlock(const String& aName, const int aBindingPoint);

    private:
        GLint GetLocationCached(const String& aName);

        std::vector<GpuShaderStageOGL*> mStages;
        UniformCacheType mUniformCache{};
        GLuint mProgramId;
    };

	class GpuShaderStageOGL : public GpuShaderStage
	{
	public:
		GpuShaderStageOGL(const GpuShaderStageType aStageType, const String& aName) : GpuShaderStage(aStageType, aName), mApiId(0) {}
		~GpuShaderStageOGL();
		void Delete();
		bool Compile();
		GLuint GetApiId() { return mApiId; }

	private:
		GLuint mApiId;
	};

}

#endif
