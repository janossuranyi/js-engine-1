#ifndef JSE_SCENE_H
#define JSE_SCENE_H

#include "system/SystemTypes.hpp"
#include "system/Filesystem.hpp"
#include "graphics/GraphicsTypes.hpp"
#include "graphics/VertexArray.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/GpuShader.hpp"
#include "graphics/ShaderManager.hpp"
#include "scene/Node3d.hpp"
#include "scene/Mesh3d.hpp"
#include "scene/Light.hpp"
#include <list>
#include <assimp/Importer.hpp>

class aiNode;
struct aiString;

namespace jse {

	class Node3d;
	class Mesh3d;
	class GraphicsDriver;

	enum RenderPass
	{
		RenderPass_Node,
		RenderPass_Z,
		RenderPass_Ambient,
		RenderPass_Light,
		RenderPass_LastEnum
	};

	struct DrawEntityDef_t
	{
		DrawEntityDef_t(const int aIndex, const MaterialType aMaterial, const Matrix& aNormalTrans, const Matrix& aModelTrans, const Matrix& aMVP) :
			mMeshIndex(aIndex),
			mMaterial(aMaterial),
			mNormalTrans(aNormalTrans),
			mModelTrans(aModelTrans),
			mMVP(aMVP) {}

		int mMeshIndex;
		MaterialType mMaterial;
		Matrix mNormalTrans;
		Matrix mModelTrans;
		Matrix mMVP;
	};

	struct FlatBufferHandle_t
	{
		FlatBufferHandle_t() {
			offset = 0;
			size = 0;
		}

		size_t offset;
		size_t size;
	};

	typedef std::vector<FlatBufferHandle_t> GpuBufferHandleVec;
	typedef std::vector<Mesh3d*> Mesh3dVec;
	typedef std::list<Light*> LightVec;
	typedef std::vector<DrawEntityDef_t> DrawEntityVec;
	typedef std::pair<const Mesh3d*, int> MeshQueryResult;

	class Scene
	{
	public:
		Scene(const String& aName, ShaderManager* aShaderManager, GraphicsDriver* aGraphDrv, FileSystem* aFileSystem);
		~Scene();

		void SetCameraPos(const Vector3f& aPos, const Vector3f& aTarget, const Vector3f& aUp);
		void SetPerspectiveCameraLens(const float aFOV, const float aAspect, const float aZNear, const float aZFar);
		void AddNode(Node3d* aNode, Node3d* aParent = nullptr);
		int AddMesh(const Mesh3d& aSrc);
		Mesh3d* GetMeshByIndex(const int aIdx);
		bool LoadScene(const String& aFileName, const bool aToYUp = false);
		bool Compile();
		void Draw(GpuShader* aShader);
		inline float GetDefaultLightRadius() const { return mDefaultLightRadius; }
		float SetDefaultLightRadius(const float a0);
		Node3d* GetNodeByName(const String& aName);
		const LightVec& GetLights() const { return mLights; }

		MeshQueryResult GetMeshByName(const String& aName);

	private:

		void processNodesRecursive(const aiNode* node, Node3d* parent, Matrix aMatrix);
		void BuildDrawList(Node3d* node);
		void DrawList();
		void DrawMesh(const unsigned aMesh) const;
		void Init();
		aiNode* FindAINodeByName(const aiString& aName, aiNode* aNode, aiMatrix4x4& aTrans);

		String mName;
		Node3d mRootNode;
		Mesh3dVec mMeshes;
		LightVec mLights;
		Light* mCurrentLight;

		GpuBufferHandleVec mVertexBufferHandles;
		GpuBufferHandleVec mIndexBufferHandles;

		GraphicsDriver* mGd;
		ShaderManager* mSm;

		FileSystem* mFileSystem;
		VertexArray* mVA;
		BufferObject* mBuffers[2];
		bool mCompiled;
		float mDefaultLightRadius;
		float mDefaultLightRadius2;

		GpuShader* mCurrentShader;

		Matrix mV, mP, mVP, mMVP;
		Vector3f mViewPos;
		RenderPass mRPass;

		DrawEntityVec mDrawList;
	};
}
#endif