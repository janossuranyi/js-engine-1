#ifndef JSE_MESH3D_H
#define JSE_MESH3D_H

#include "system/SystemTypes.hpp"
#include "graphics/GraphicsTypes.hpp"
#include "graphics/Material.hpp"
#include "graphics/Renderable.hpp"

namespace jse {

	class VertexData
	{
	public:

		VertexData();

		void SetPosition(const float* a0);
		void SetPosition(const float aX, const float aY, const float aZ);
		void SetNormal(const float* a0);
		void SetNormal(const float aX, const float aY, const float aZ);
		void SetTangent(const float* a0);
		void SetTangent(const float aX, const float aY, const float aZ);
		void SetBiTangent(const float* a0);
		void SetBiTangent(const float aX, const float aY, const float aZ);
		void SetTexCoord(const float* a0);
		void SetTexCoord(const float aS, const float aT);

		Vector3f position;
		Vector3f normal;
		Vector3f tangent;
		Vector3f bitangent;
		Vector2f texcoord;
		float pad[2]; // padding to 64 byte
	};

	const unsigned int kVertexDataSize = sizeof(VertexData);

	typedef std::vector<VertexData> VertexDataVec;
	typedef VertexDataVec::iterator VertexDataVecIt;
	typedef std::vector<unsigned short> ShortPrimitiveIndices;
	typedef std::vector<unsigned int> LongPrimitiveIndices;

	class Mesh3d : public Renderable
	{
		friend class Scene;
	public:
		Mesh3d(const String& aName);
		~Mesh3d();
		void SetName(const String& aName);
		void AddVertex(const VertexData& a0);
		void AddIndex(const unsigned short aIdx);
		void AddIndices(const unsigned short* aIndices, const unsigned aSize);
		void SetMaterial(const Material& aMat) { mMaterial = aMat; }
		void ClearData();
		void SetData(const vec3* aPositions, const vec3* aNormals, const vec4* aTangents, const vec2* aTexcoords, const size_t aCount);

		void CompileFromData();
		void SetIndex(const unsigned int a0) { mIndex = a0; }
		unsigned int GetIndex() const { return mIndex; }
		
		RenderableType GetType() const { return RenderableType::Mesh; }

		void Draw() {};

	private:


		String mName;
		VertexDataVec vertices;
		ShortPrimitiveIndices indices;
		Material mMaterial;
		unsigned int mIndex;

		size_t mDataCount{};
		vec3* mPositionData{};
		vec3* mNormalData{};
		vec4* mTangentData{};
		vec2* mTexcoordData{};

	};

}
#endif