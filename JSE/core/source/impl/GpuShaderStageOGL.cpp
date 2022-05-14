#include <GL/glew.h>
#include <memory>
#include "impl/GraphicsDriverOGL.hpp"
#include "impl/GpuShaderOGL.hpp"
#include "system/Logger.hpp"

namespace jse {

	bool GpuShaderStageOGL::Compile()
	{
		if (mCompiled)
			return true;

		if (mSource.empty())
		{
			Error("Shader error: no source code loaded !");
			return false;
		}

		if (!mApiId)
		{
			mApiId = glCreateShader(GetGLShaderStageTypeEnum(mStage));
			if (!mApiId)
			{
				Error("Cannot create GL shader!");
				return false;
			}
		}

		GLint result = GL_FALSE;

		char const* source_cstr = mSource.c_str();
		glShaderSource(mApiId, 1, &source_cstr, nullptr);
		glCompileShader(mApiId);
		glGetShaderiv(mApiId, GL_COMPILE_STATUS, &result);

		if (result == GL_FALSE)
		{
			GLint infologLen;
			glGetShaderiv(mApiId, GL_INFO_LOG_LENGTH, &infologLen);
			if (infologLen > 0) {
				std::vector<char> logBuf(infologLen);
				glGetShaderInfoLog(mApiId, infologLen, nullptr, logBuf.data());
				Error("Compilation of %s failed: %s", mName.c_str(), logBuf.data());
			}

			glDeleteShader(mApiId);
			mApiId = 0;

			return false;
		}

		mCompiled = true;

		return true;
	}

	void GpuShaderStageOGL::Delete()
	{
		if (mApiId)
		{
			glDeleteShader(mApiId);
			mApiId = 0;
			mCompiled = false;
		}
	}

	GpuShaderStageOGL::~GpuShaderStageOGL()
	{
		Delete();
		Info("GpuShaderStageOGL deleted");
	}
}