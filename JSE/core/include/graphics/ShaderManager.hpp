#ifndef JSE_SHADER_MANAGER_H
#define JSE_SHADER_MANAGER_H

#include "graphics/GraphicsTypes.hpp"
#include "graphics/GraphicsDriver.hpp"
#include "system/SystemTypes.hpp"
#include "system/Filesystem.hpp"
#include "graphics/GpuShader.hpp"

#include <map>

namespace jse {

	struct ShaderDef
	{
		String mName;
		GpuShaderStageType mType;
	};

	struct ProgramDef
	{
		MaterialType mMaterial;
		String mName;
		String mVertShader;
		String mFragShader;
		String mGeomShader;
		String mCompShader;
	};

	struct MaterialProgram
	{
		MaterialType mMaterial;
		String mProgram;
	};

	typedef std::map<String, GpuShaderStage*> tStageByName;
	typedef tStageByName::value_type tStageByNamePair;
	typedef std::map<String, GpuShader*> tProgramByName;
	typedef tProgramByName::value_type tProgramByNamePair;
	typedef std::map<MaterialType, GpuShader*> tMaterialShader;
	typedef tMaterialShader::value_type tMaterialShaderPair;

	class ShaderManager
	{
	public:
		ShaderManager(GraphicsDriver* aGraphicsDriver, FileSystem* aFileSystem);
		~ShaderManager();
		bool Init();
		GpuShader* GetShaderByName(const String& aName);
		GpuShader* GetShaderByMaterial(const MaterialType aType);

	private:
		tProgramByName mShaderByNameMap;
		tStageByName mStageByName;
		GraphicsDriver* mGraphicsDriver;
		FileSystem* mFileSystem;
		tMaterialShader mMaterialShaderMap;
	};
}
#endif