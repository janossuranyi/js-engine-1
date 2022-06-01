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
#include "scene/AnimationManager.hpp"

#include <list>
#include <map>
#include <memory>
#include <functional>
#include <assimp/Importer.hpp>

struct aiNode;
struct aiString;

namespace jse {

	class Node3d;
	class Mesh3d;
	class GraphicsDriver;
	class GltfLoader;

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
		DrawEntityDef_t(Mesh3d* aPtr, const MaterialType aMaterial, const Matrix& aNormalTrans, const Matrix& aModelTrans, const Matrix& aMVP) :
			mPtr(aPtr),
			mMaterial(aMaterial),
			mNormalTrans(aNormalTrans),
			mModelTrans(aModelTrans),
			mMVP(aMVP) {}

		Mesh3d* mPtr;
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
	typedef std::vector<std::shared_ptr<Mesh3d>> Mesh3dVec;
	typedef std::list<Light*> LightVec;
	typedef std::vector<DrawEntityDef_t> DrawEntityVec;
	typedef std::pair<const Mesh3d*, int> MeshQueryResult;

	class Scene
	{
		friend class AssimpLoader;
		friend class GltfLoader;

	public:
		Scene(const String& aName, ShaderManager* aShaderManager, GraphicsDriver* aGraphDrv, FileSystem* aFileSystem);
		~Scene();

		void SetCameraPos(const Vector3f& aPos, const Vector3f& aTarget, const Vector3f& aUp);
		void SetPerspectiveCameraLens(const float aFOV, const float aAspect, const float aZNear, const float aZFar);
		void AddNode(Node3d* aNode, Node3d* aParent = nullptr);
		size_t AddMesh(const Mesh3d& aSrc);
		std::shared_ptr<Mesh3d> GetMeshByIndex(const int aIdx);
		bool LoadScene(const String& aFileName, const bool aToYUp = false);
		bool Compile();
		void Draw();
		inline float GetDefaultLightRadius() const { return mDefaultLightRadius; }
		float SetDefaultLightRadius(const float a0);
		Node3d* GetNodeByName(const String& aName);

		void UpdateAnimation(const float aFrameStep);

		MeshQueryResult GetMeshByName(const String& aName);

		void WalkNodeHiearchy(std::function<void(Node3d*)> func);

	private:

		void BuildDrawList(Node3d* node);
		void DrawList();
		void DrawMesh(const Mesh3d* aMesh);
		void Init();
		aiNode* FindAINodeByName(const aiString& aName, aiNode* aNode, aiMatrix4x4& aTrans);

		AnimationManager mAnimMgr;

		String mName;
		Node3d mRootNode;
		Mesh3dVec mMeshes;
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

		std::map<String, Node3d*> mNodeByName;

		typedef std::map<String, Node3d*>::value_type tNodeByNamePair;

		int m_drawCallsPerFrame{ 0 };
		int m_stateChangePerFrame{ 0 };
		int m_sampleCount{ 0 };
	};
}
#endif