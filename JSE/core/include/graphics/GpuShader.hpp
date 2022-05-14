#ifndef JSE_GPU_SHADER_H
#define JSE_GPU_SHADER_H

#include <memory>
#include "graphics/GraphicsTypes.hpp"

/*


================================
Vertex attribute names

Attrib name		size
va_Position		3
va_Normal		3
va_Tangent		3
va_Texcoord0	2
================================

Uniform buffers

layout(std140) uniform Matrices {
	mat4 M;
	mat4 V;
	mat4 P;
	mat4 MV;
	mat4 MVP;
}

struct Light {
	vec3 position;
	vec3 color;
}

uniform Light g_Lights[]
uniform vec3 g_CameraPos
uniform vec3 g_CameraDir


*/ 
namespace jse {

	class GpuShaderStage
	{
	public:

		GpuShaderStage(const GpuShaderStageType aStageTye, const String& aName) :
			mStage(aStageTye),
			mName(aName),
			mCompiled(false) {};

		virtual ~GpuShaderStage() {};

		GpuShaderStageType GetStageType() const { return mStage; }
		inline void SetSource(const String& aSource) { mSource = aSource; }
		inline bool isCompiled() const { return mCompiled; }
		virtual bool Compile() = 0;
		virtual void Delete() = 0;
	protected:
		GpuShaderStageType mStage;
		String mSource{};
		String mName;
		bool mCompiled;
	};

	class GpuShader
	{
	public:
		GpuShader() : mCompiled(false) {}
		virtual ~GpuShader() {};
		virtual void AddStage(const GpuShaderStageType aStage, const String& aSourceName, const String& aName) = 0;
		virtual void AddStage(GpuShaderStage* aShader) = 0;
		virtual bool Compile() = 0;
		virtual void Delete() = 0;
		inline bool isCompiled() const { return mCompiled; }

		virtual void Use() = 0;

		virtual void SetFloat(const String& aName, const float aVal) = 0;
		virtual void SetInt(const String& aName, const int aVal) = 0;
		virtual void SetFloatv(const String& aName, const size_t aSize, const float* aVal) = 0;
		virtual void BindUniformBlock(const String& aName, const int aBindingPoint) = 0;
		
		virtual void SetVector3(const String&, const float*) = 0;
		virtual void SetVector4(const String&, const float*) = 0;
		
		virtual void SetMatrix(const String&, const float*) = 0;
		virtual void SetMatrix3(const String&, const float*) = 0;
		/*
		void SetMatrix4x4(const std::string, const float*);
		*/
	protected:
		bool mCompiled;
	};

}

#endif