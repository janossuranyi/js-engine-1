#include <GL/glew.h>
#include <memory>
#include "system/Logger.hpp"
#include "impl/GpuShaderOGL.hpp"

namespace jse {


	void GpuShaderOGL::AddStage(GpuShaderStage* aShader)
	{
		GpuShaderStageOGL* ptr = reinterpret_cast<GpuShaderStageOGL*>(aShader);

		mStages.push_back(ptr);
	}

	void GpuShaderOGL::AddStage(const GpuShaderStageType aStage, const String& aSource, const String& aName)
	{
		GpuShaderStageOGL* st = new GpuShaderStageOGL(aStage, aName);
		st->SetSource(aSource);

		mStages.push_back(st);
	}

	bool GpuShaderOGL::Compile()
	{
		mCompiled = false;

		if (!mProgramId)
		{
			mProgramId = glCreateProgram();
		}

		for (auto it : mStages)
		{
			if (!it->Compile()) {
				return false;
			}
		}

		GLint result = GL_FALSE;

		for (auto it : mStages)
		{
			glAttachShader(mProgramId, it->GetApiId());
		}

		Info("Linking program...");

		glLinkProgram(mProgramId);

		glGetProgramiv(mProgramId, GL_LINK_STATUS, &result);
		if (result == GL_FALSE)
		{
			GLint infologLen;
			glGetProgramiv(mProgramId, GL_INFO_LOG_LENGTH, &infologLen);
			if (infologLen > 0) {
				std::vector<char> logBuf(infologLen);
				glGetProgramInfoLog(mProgramId, infologLen, nullptr, logBuf.data());
				Error("Linking of shader program failed: %s", logBuf.data());

				return false;
			}
		}

		mCompiled = true;

		return true;
	}

	void GpuShaderOGL::Use()
	{
		if (mCompiled)
		{
			glUseProgram(mProgramId);
		}
	}

	void GpuShaderOGL::SetFloat(const String& aName, const float aVal)
	{
		if (mProgramId)
		{
			GLint location = GetLocationCached(aName);

			if (location > -1)
			{
				glUniform1f(location, aVal);
			}
		}
	}

	void GpuShaderOGL::SetFloatv(const String& aName, const size_t aSize, const float* aVal)
	{
		if (mProgramId)
		{
			GLint location = GetLocationCached(aName);

			if (location > -1)
			{
				glUniform1fv(location, aSize, aVal);
			}
		}
	}

	void GpuShaderOGL::SetInt(const String& aName, const int aVal)
	{
		if (mProgramId)
		{
			GLint location = GetLocationCached(aName);

			if (location > -1)
			{
				glUniform1i(location, aVal);
			}
		}
	}

	void GpuShaderOGL::SetVector3(const String& aName, const float* aData)
	{
		if (mProgramId)
		{
			GLint location = GetLocationCached(aName);
			if (location > -1)
			{
				glUniform3fv(location, 1, aData);
			}
		}
	}

	void GpuShaderOGL::SetVector4(const String& aName, const float* aData)
	{
		if (mProgramId)
		{
			GLint location = GetLocationCached(aName);
			if (location > -1)
			{
				glUniform4fv(location, 1, aData);
			}
		}
	}

	void GpuShaderOGL::SetMatrix(const String& aName, const float* aData)
	{
		if (mProgramId)
		{
			GLint location = GetLocationCached(aName);
			if (location > -1)
			{
				glUniformMatrix4fv(location, 1, GL_FALSE, aData);
			}
		}
	}

	void GpuShaderOGL::SetMatrix3(const String& aName, const float* aData)
	{
		if (mProgramId)
		{
			GLint location = GetLocationCached(aName);
			if (location > -1)
			{
				glUniformMatrix3fv(location, 1, GL_FALSE, aData);
			}
		}
	}

	void GpuShaderOGL::BindUniformBlock(const String& aName, const int aBindingPoint)
	{
		GLuint block_index = glGetUniformBlockIndex(mProgramId, aName.c_str());

		if (GL_INVALID_INDEX == block_index) {
			Error("Uniform  block '%s' not exists", aName.c_str());
			return;
		}

		glUniformBlockBinding(mProgramId, block_index, aBindingPoint);
	}

	GLint GpuShaderOGL::GetLocationCached(const String& aName)
	{
		GLint location = -1;
		if (mProgramId)
		{
			auto mapRes = mUniformCache.find(aName);
			if (mapRes != mUniformCache.end()) {
				location = mapRes->second;
			} 
			else
			{
				location = glGetUniformLocation(mProgramId, aName.c_str());
				mUniformCache.insert(UniformCacheType::value_type(aName, location));
			}
		}

		return location;
	}

	void GpuShaderOGL::Delete()
	{
		if (mProgramId)
		{
			glDeleteProgram(mProgramId);
		}

		mStages.clear();
	}

	GpuShaderOGL::~GpuShaderOGL()
	{
		Delete();
	}

}