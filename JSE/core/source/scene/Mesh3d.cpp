#include "scene/Mesh3d.hpp"
#include "scene/Node3d.hpp"
#include "system/Logger.hpp"


#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

namespace jse {


	VertexData::VertexData()
	{
	}

	void VertexData::SetPosition(const float* a0)
	{
		position[0] = a0[0];
		position[1] = a0[1];
		position[2] = a0[2];
	}

	void VertexData::SetPosition(const float aX, const float aY, const float aZ)
	{
		position.x = aX;
		position.y = aY;
		position.z = aZ;
	}

	void VertexData::SetNormal(const float* a0)
	{
		normal[0] = a0[0];
		normal[1] = a0[1];
		normal[2] = a0[2];
	}

	void VertexData::SetNormal(const float aX, const float aY, const float aZ)
	{
		normal.x = aX;
		normal.y = aY;
		normal.z = aZ;
	}

	void VertexData::SetTangent(const float* a0)
	{
		tangent[0] = a0[0];
		tangent[1] = a0[1];
		tangent[2] = a0[2];
	}

	void VertexData::SetTangent(const float aX, const float aY, const float aZ)
	{
		tangent.x = aX;
		tangent.y = aY;
		tangent.z = aZ;

	}

	void VertexData::SetBiTangent(const float* a0)
	{
		bitangent[0] = a0[0];
		bitangent[1] = a0[1];
		bitangent[2] = a0[2];
	}

	void VertexData::SetBiTangent(const float aX, const float aY, const float aZ)
	{
		bitangent.x = aX;
		bitangent.y = aY;
		bitangent.z = aZ;
	}

	void VertexData::SetTexCoord(const float* a0)
	{
		texcoord[0] = a0[0];
		texcoord[1] = a0[1];
		texcoord[2] = a0[2];
	}

	void VertexData::SetTexCoord(const float aS, const float aT)
	{
		texcoord.s = aS;
		texcoord.t = aT;
	}


	Mesh3d::Mesh3d(const String& aName) : mName(aName)
	{
	}

	void Mesh3d::SetName(const String& aName)
	{
		mName = aName;
	}

	void Mesh3d::AddVertex(const VertexData& a0)
	{
		vertices.push_back(a0);
	}

	void Mesh3d::AddIndex(const unsigned short aIdx)
	{
		indices.push_back(aIdx);
	}


}