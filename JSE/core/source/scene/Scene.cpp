#include <algorithm>
#include <vector>
#include <stack>
#include <memory>
#include <functional>

#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene/Scene.hpp"
#include "scene/Mesh3d.hpp"
#include "scene/Node3d.hpp"
#include "scene/Animation.hpp"
#include "scene/AnimationTrack.hpp"
#include "scene/GltfLoader.hpp"
#include "system/Logger.hpp"
#include "system/Strings.hpp"

#include "graphics/GraphicsDriver.hpp"
#include "graphics/GpuShader.hpp"
#include "graphics/Renderable.hpp"

#define SCENE_ALIGN16(x) (((x) + 15) & ~15)

namespace jse {


	bool Scene_MeshOrderComparator(const DrawEntityDef_t& a0, const DrawEntityDef_t& a1)
	{
		return a0.mMaterial < a1.mMaterial;
	}


	Scene::Scene(const String& aName, ShaderManager* aShaderManager, GraphicsDriver* aGraphDrv, FileSystem* aFileSystem)
	{
		mName = aName;
		mGd = aGraphDrv;
		mFileSystem = aFileSystem;
		mCompiled = false;
		mRootNode = Node3d("ROOT");
		mRootNode.SetVisible(true);
		mCurrentShader = nullptr;
		mSm = aShaderManager;
		mDefaultLightRadius = 1.0;
		mDefaultLightRadius2 = 1.0;

		mV = mP = mVP = mMVP = Matrix(1.0f);

		Init();
	}

	Scene::~Scene()
	{
		delete mLightsBuffer;
		if (mBuffers[0]) delete mBuffers[0];
		if (mBuffers[1]) delete mBuffers[1];
		if (mVA) delete mVA;
	}

	void Scene::UpdateLights()
	{
		// indexing lights
		mUniformLights.clear();
		mLights.clear();
		mLightIndexMap.clear();

		WalkNodeHiearchy([&](Node3d* n) {

			n->UpdateWorldTransform();

			for (auto r : n->GetRenderables())
			{
				if (r->GetType() == RenderableType::Light)
				{
					unsigned idx = mLights.size();
					auto light = std::dynamic_pointer_cast<Light>(r);
					const PointLight* pl = reinterpret_cast<PointLight*>(light.get());

					mLights.push_back(r);

					UniformLight ul{
						vec4(n->GetWorldPosition(), 1.0f),
						vec4(pl->GetDiffuse(), 1.0f),
						vec4(pl->GetSpecular(), 1.0f),
						pl->GetLinearAtt(),
						pl->GetQuadraticAtt(),
						pl->GetCutOff() };

					mUniformLights.push_back(ul);
					mLightIndexMap.insert(std::make_pair(r, idx));
				}
			}
		});

		mLightsBuffer->BindToIndex(0);
		mLightsBuffer->Reset();
		mLightsBuffer->Alloc(mUniformLights.size() * sizeof(UniformLight), mUniformLights.data());
	}

	void Scene::UpdateCamera()
	{
		mV = mCamera.GetViewMatrix();
		mViewPos = mCamera.GetPosition();
		mVP = mP * mV;
	}

	void Scene::SetPerspectiveCameraLens(const float aFOV, const float aAspect, const float aZNear, const float aZFar)
	{
		mP = glm::perspective(aFOV, aAspect, aZNear, aZFar);
		mVP = mP * mV;
	}

	void Scene::AddNode(Node3d* aNode, Node3d* aParent)
	{
		if (aParent)
			aParent->AddChildNode(aNode);
		else
			mRootNode.AddChildNode(aNode);
	}

	size_t Scene::AddMesh(const Mesh3d& aSrc)
	{
		auto newMesh = std::make_shared<Mesh3d>(aSrc.mName);
		newMesh->vertices = aSrc.vertices;
		newMesh->indices = aSrc.indices;
		newMesh->mMaterial = aSrc.mMaterial;

		const size_t res = mMeshes.size();
		newMesh->SetIndex(res);

		mMeshes.push_back(newMesh);

		return res;
	}

	std::shared_ptr<Mesh3d> Scene::GetMeshByIndex(const int aIdx)
	{
		if (mMeshes.size() > aIdx)
		{
			return mMeshes.at(aIdx);
		}

		return nullptr;
	}

	bool Scene::LoadScene(const String& aFileName, const bool aToYUp)
	{
		std::unique_ptr<SceneLoader> loader = std::make_unique<GltfLoader>(*this);

		int res = loader->LoadScene(aFileName);
		if (res != 0) return res;

		
		UpdateLights();

		Info("Scene loaded!");
	}

	bool Scene::Compile()
	{
		if (mCompiled)
		{
			delete mVA;
			delete mBuffers[0];
			delete mBuffers[1];
			mIndexBufferHandles.clear();
			mVertexBufferHandles.clear();
		}

		mCompiled = false;

		assert((kVertexDataSize & 15) == 0);

		size_t l_requiredVertexBufferSize = 0;
		size_t l_requiredIndexBufferSize = 0;

		for (unsigned int i = 0; i < mMeshes.size(); i++)
		{
			l_requiredVertexBufferSize += (mMeshes[i]->vertices.size() * kVertexDataSize);
			l_requiredIndexBufferSize += SCENE_ALIGN16(mMeshes[i]->indices.size() * sizeof(unsigned short));
		}

		mBuffers[0] = mGd->CreateBuffer(BufferTarget_Vertex, BufferUsage_StaticDraw, l_requiredVertexBufferSize);
		mBuffers[1] = mGd->CreateBuffer(BufferTarget_Index, BufferUsage_StaticDraw, l_requiredIndexBufferSize);

		BufferObject* vb = mBuffers[0];
		BufferObject* ib = mBuffers[1];

		vb->Bind();
		ib->Bind();

		for (int i = 0; i < mMeshes.size(); i++)
		{
			auto m = mMeshes[i];
			FlatBufferHandle_t vtxH, idxH;

			vtxH.size = m->vertices.size() * kVertexDataSize;
			vtxH.offset = vb->GetAlloced();
			vb->Alloc(vtxH.size, m->vertices.data());

			idxH.size = SCENE_ALIGN16( m->indices.size() * sizeof(unsigned short) );
			idxH.offset = ib->GetAlloced();
			ib->Alloc(idxH.size, nullptr);
			ib->UpdateData(idxH.offset, m->indices.size() * sizeof(unsigned short), m->indices.data());

			mIndexBufferHandles.push_back(idxH);
			mVertexBufferHandles.push_back(vtxH);
		}

		
		// Create Vertex array
		VertexArrayAttributes vAttr;
		vAttr.AddVertexAttrib(VertexBufferElement_Position,	VtxAttribType_Float, 3, kVertexDataSize, 0, vb);
		vAttr.AddVertexAttrib(VertexBufferElement_Normal,	VtxAttribType_Float, 3, kVertexDataSize, 0, vb);
		vAttr.AddVertexAttrib(VertexBufferElement_Tangent,	VtxAttribType_Float, 3, kVertexDataSize, 0, vb);
		vAttr.AddVertexAttrib(VertexBufferElement_BiTangent,VtxAttribType_Float, 3, kVertexDataSize, 0, vb);
		vAttr.AddVertexAttrib(VertexBufferElement_Texture0,	VtxAttribType_Float, 2, kVertexDataSize, 0, vb);
		mVA = mGd->CreateVertexArray(ib, vAttr);

		mVA->Compile();


		mCompiled = true;

		return true;
	}

	void Scene::Draw()
	{

		//if (m_sampleCount++ > 100) {
		//	Info("m_drawCallsPerFrame: %d, m_stateChangePerFrame: %d", m_drawCallsPerFrame, m_stateChangePerFrame);
		//	m_sampleCount = 0;
		//}

		m_drawCallsPerFrame = 0;
		m_stateChangePerFrame = 0;

		mDrawList.clear();
		BuildDrawList(&mRootNode);
		std::sort(mDrawList.begin(), mDrawList.end(), Scene_MeshOrderComparator);


		/************************************
		Render Z-Pass
		*************************************/

		mVA->Bind();

		mGd->SetCullFaceEnable(true);

		mRPass = RenderPass_Z;
		mGd->SetDepthMask(true);
		mGd->SetDepthTestEnable(true);
		mGd->SetDepthTestFunc(DepthTestFunc_Less);
		mGd->SetClearDepth(1.0f);
		mGd->ClearFrameBuffer(ClearFBFlags_Depth);
		mGd->SetColorMask(ColorMask_None);

		DrawList();

		/************************************
		Render Lights
		*************************************/

		mGd->SetDepthMask(false);
		mGd->SetColorMask(ColorMask_RGBA);
		mGd->ClearFrameBuffer(ClearFBFlags_Color);
		mGd->SetDepthTestFunc(DepthTestFunc_Equal);
		mRPass = RenderPass_Light;

		DrawList();

		mGd->UseShader(NULL);

	}

	float Scene::SetDefaultLightRadius(const float a0)
	{
		float prev = mDefaultLightRadius;
		mDefaultLightRadius = a0;
		mDefaultLightRadius2 = a0 * a0;

		//Info("Light attenuation: %.2f %.2f %.2f", 1.0f, 2.0f / mDefaultLightRadius, 1.0f / mDefaultLightRadius2);

		for (auto l : mLights)
		{
			auto light = std::dynamic_pointer_cast<PointLight>(l);
			light->SetAttenuation(2.0f / mDefaultLightRadius, 1.0f / mDefaultLightRadius2);
		}

		return prev;
	}

	Node3d* Scene_recurse_search(Node3d* aNode, const String& aName)
	{
		if (aNode->GetName() == aName)
			return aNode;

		for (auto it = aNode->GetChildren().begin(); it != aNode->GetChildren().end(); it++)
		{
			Node3d* tmp = Scene_recurse_search(*it, aName);
			if (tmp)
				return tmp;
		}

		return nullptr;
	}

	Node3d* Scene::GetNodeByName(const String& aName)
	{
		//return Scene_recurse_search(&mRootNode, aName);
		auto it = mNodeByName.find(aName);
		if (it == mNodeByName.end())
			return nullptr;

		return it->second;
	}

	void Scene::UpdateAnimation(const float aFrameStep)
	{
		mAnimMgr.UpdateState(aFrameStep);
		UpdateLights();
	}

	MeshQueryResult Scene::GetMeshByName(const String& aName)
	{
		for (int i = 0; i < mMeshes.size(); i++)
		{
			if (mMeshes[i]->mName == aName)
				return MeshQueryResult(mMeshes[i].get(), i);
		}

		return MeshQueryResult(nullptr, -1);
	}

	void Scene::WalkNodeHiearchy(std::function<void(Node3d*)> func)
	{
		std::stack<Node3d*> stk;

		stk.push(&mRootNode);

		while (!stk.empty())
		{
			Node3d* n = stk.top();

			func(n);

			stk.pop();

			for (auto e : n->GetChildren())
			{
				stk.push(e);
			}
		}
	}


	void Scene::BuildDrawList(Node3d* node)
	{
		if (!node->IsVisible())
			return;

		node->UpdateWorldTransform();

		if (node->HasChildren())
		{
			for (auto& it : node->GetChildren())
			{
				BuildDrawList(it);
			}
		}

		const Matrix& M = node->GetWorldMatrix();
		const Matrix NM = Matrix3x3(glm::transpose(glm::inverse(M)));
		const Matrix MVP = mP * mV * M;

		for (auto renderable : node->GetRenderables())
		{
			if (renderable->GetType() != RenderableType::Mesh)
				continue;

			Mesh3d* mesh = reinterpret_cast<Mesh3d*>(renderable.get());

			mDrawList.emplace_back(mesh, mesh->mMaterial.type, NM, M, MVP);
		}
	}

	void Scene::DrawList()
	{
		MaterialType mtCurrent = mRPass == RenderPass_Z ? MaterialType_ZPass : MaterialType_LastEnum;
		mCurrentShader = nullptr; 

		const Vector3f cBlack(0.0f);

		if (mRPass == RenderPass_Z)
		{
			mCurrentShader = mSm->GetShaderByMaterial(mtCurrent);
			mCurrentShader->Use();
			mCurrentShader->SetInt("numLights", 0);
			mCurrentShader->SetVector3("viewPos", &mViewPos[0]);
		}

		for (auto it = mDrawList.begin(); it != mDrawList.end(); it++)
		{
			DrawEntityDef_t ent = *it;
			
			if (mtCurrent != ent.mMaterial && mtCurrent != MaterialType_ZPass)
			{
				m_stateChangePerFrame++;

				mtCurrent = ent.mMaterial;
				mCurrentShader = mSm->GetShaderByMaterial(mtCurrent);
				mCurrentShader->Use();
				mCurrentShader->SetVector3("viewPos", &mViewPos[0]);
				mCurrentShader->BindUniformBlock("LightBuffer", 0);

				if (mRPass == RenderPass_Light)
				{
					mCurrentShader->SetInt("numLights", mUniformLights.size());
				}
			}

			mCurrentShader->SetMatrix("M", &ent.mModelTrans[0][0]);
			mCurrentShader->SetMatrix("NM", &ent.mNormalTrans[0][0]);
			mCurrentShader->SetMatrix("MVP", &ent.mMVP[0][0]);


			DrawMesh(ent.mPtr);
		}
	}

	void Scene::DrawMesh(const Mesh3d* aMesh)
	{
		const Mesh3d* m = aMesh;
		const Vector3f kBlack(0.0f);

		FlatBufferHandle_t vtxH = mVertexBufferHandles[aMesh->GetIndex()];
		FlatBufferHandle_t idxH = mIndexBufferHandles[aMesh->GetIndex()];
		size_t baseVert = vtxH.offset / sizeof(VertexData);

		if (mRPass == RenderPass_Light)
		{
			mCurrentShader->SetVector3("material.ambient", &m->mMaterial.ambient[0]);
			mCurrentShader->SetVector3("material.diffuse", &m->mMaterial.diffuse[0]);
			mCurrentShader->SetVector3("material.specular", &m->mMaterial.specular[0]);
			mCurrentShader->SetFloat("material.shininess", m->mMaterial.specularIntesity);
		}
		else
		{
			mCurrentShader->SetVector3("material.ambient", &m->mMaterial.ambient[0]);
			mCurrentShader->SetVector3("material.diffuse", &kBlack[0]);
			mCurrentShader->SetVector3("material.specular", &kBlack[0]);
			mCurrentShader->SetFloat("material.shininess", 1.0f);
		}

		m_drawCallsPerFrame++;

		mGd->DrawPrimitivesWithBase(PrimitiveType_Triangles, m->indices.size(), idxH.offset, baseVert);
	}

	void Scene::Init()
	{
		mLightsBuffer = mGd->CreateBuffer(BufferTarget_Uniform, BufferUsage_DynaDraw, 256ULL*64);
	}
}
