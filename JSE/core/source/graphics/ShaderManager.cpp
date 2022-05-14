#include "graphics/ShaderManager.hpp"
#include "graphics/GraphicsTypes.hpp"
#include "system/Logger.hpp"

namespace jse {

	const ShaderDef shader_defs[] = {
		{"zpass_vtx", ShaderStage_Vertex},
		{"zpass_frag", ShaderStage_Fragment},
		{"specular_vtx", ShaderStage_Vertex},
		{"specular_frag", ShaderStage_Fragment}
//		{"diffuse_vtx", ShaderStage_Vertex},
//		{"diffuse_frag", ShaderStage_Fragment}
	};

	const ProgramDef program_defs[] = {
		{MaterialType_ZPass, "zpass", "zpass_vtx", "zpass_frag", "", ""},
		{MaterialType_Specular, "specular", "specular_vtx", "specular_frag", "", ""}
//		{MaterialType_Diffuse, "diffuse", "diffuse_vtx", "diffuse_frag", "", ""}
	};

	ShaderManager::ShaderManager(GraphicsDriver* aGraphicsDriver, FileSystem* aFileSystem)
	{
		mGraphicsDriver = aGraphicsDriver;
		mFileSystem = aFileSystem;
	}

	ShaderManager::~ShaderManager()
	{
		for (auto it = mStageByName.begin(); it != mStageByName.end(); it++)
		{
			delete it->second;
		}

		for (auto it = mShaderByNameMap.begin(); it != mShaderByNameMap.end(); it++)
		{
			delete it->second;
		}
	}

	bool ShaderManager::Init()
	{
		// Loading shader stages
		for (int i = 0; i < sizeof(shader_defs) / sizeof(ShaderDef); i++)
		{
			GpuShaderStage* stage = mGraphicsDriver->CreateGpuShaderStage(shader_defs[i].mType, shader_defs[i].mName);
			String source;
			if (!mFileSystem->ReadTextFileBase("shaders/" + shader_defs[i].mName + ".glsl", source))
			{
				Error("Shader stage %s not found !", shader_defs[i].mName.c_str());
				return false;
			}

			stage->SetSource(source);
			if (!stage->Compile())
			{
				Error("Shader stage %s not compiled !", shader_defs[i].mName.c_str());
				return false;
			}

			mStageByName.insert(tStageByNamePair(shader_defs[i].mName, stage));
		}

		for (int i = 0; i < sizeof(program_defs) / sizeof(ProgramDef); i++)
		{
			GpuShader* program = mGraphicsDriver->CreateGpuShader();
			if (!program_defs[i].mVertShader.empty())
			{
				auto it = mStageByName.find(program_defs[i].mVertShader);
				if (it != mStageByName.end())
				{
					program->AddStage(it->second);
				}
			}

			if (!program_defs[i].mFragShader.empty())
			{
				auto it = mStageByName.find(program_defs[i].mFragShader);
				if (it != mStageByName.end())
				{
					program->AddStage(it->second);
				}
			}

			if (!program->Compile())
			{
				Error("Program %s not compile !", program_defs[i].mName.c_str());
				return false;
			}

			mShaderByNameMap.insert(tProgramByNamePair(program_defs[i].mName, program));
			mMaterialShaderMap.insert(tMaterialShaderPair(program_defs[i].mMaterial, program));
		}

		
	}

	GpuShader* ShaderManager::GetShaderByName(const String& aName)
	{
		auto it = mShaderByNameMap.find(aName);
		if (it != mShaderByNameMap.end())
		{
			return it->second;
		}
		else
		{
			return nullptr;
		}
	}
	
	GpuShader* ShaderManager::GetShaderByMaterial(const MaterialType aType)
	{
		auto it = mMaterialShaderMap.find(aType);
		if (it != mMaterialShaderMap.end())
		{
			return it->second;
		}
		else
		{
			return nullptr;
		}
	}
}
