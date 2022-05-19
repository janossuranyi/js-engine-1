#include <algorithm>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/pbrmaterial.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene/Scene.hpp"
#include "scene/Mesh3d.hpp"
#include "scene/Node3d.hpp"
#include "scene/Animation.hpp"
#include "scene/AnimationTrack.hpp"
#include "system/Logger.hpp"
#include "system/Strings.hpp"

#include "graphics/GraphicsDriver.hpp"
#include "graphics/GpuShader.hpp"


#define SCENE_ALIGN16(x) (((x) + 15) & ~15)

namespace jse {

	static inline Vector3f color3_cast(const aiColor3D& v) { return Vector3f(v.r, v.g, v.b); }
	static inline Vector3f vec3_cast(const aiVector3D& v) { return Vector3f(v.x, v.y, v.z); }
	static inline Quat quat_cast(const aiQuaternion& q) { return Quat(q.w, q.x, q.y, q.z); }
	static inline Vector2f vec2_cast(const aiVector3D& v) { return Vector2f(v.x, v.y); } // it's aiVector3D because assimp's texture coordinates use that
	static inline Matrix mat4_cast(const aiMatrix4x4& m) { return glm::transpose(glm::make_mat4(&m.a1)); }

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

		if (!mMeshes.empty())
		{
			for (auto it = mMeshes.begin(); it != mMeshes.end(); it++)
			{
				delete* it;
			}
		}

		if (!mLights.empty())
		{
			for (auto it = mLights.begin(); it != mLights.end(); it++)
			{
				delete* it;
			}
		}
	}

	void Scene::SetCameraPos(const Vector3f& aPos, const Vector3f& aTarget, const Vector3f& aUp)
	{
		mV = glm::lookAt(aPos, aTarget, aUp);
		mViewPos = aPos;
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

	int Scene::AddMesh(const Mesh3d& aSrc)
	{
		Mesh3d* newMesh = new Mesh3d(aSrc.mName);
		newMesh->vertices = aSrc.vertices;
		newMesh->indices = aSrc.indices;
		newMesh->mMaterial = aSrc.mMaterial;

		const int res = mMeshes.size();
		mMeshes.push_back(newMesh);

		return res;
	}

	Mesh3d* Scene::GetMeshByIndex(const int aIdx)
	{
		if (mMeshes.size() > aIdx)
		{
			return mMeshes.at(aIdx);
		}

		return nullptr;
	}

	bool Scene::LoadScene(const String& aFileName, const bool aToYUp)
	{
		const float v_togl[16] = {
			 1.0f,  0.0f,  0.0f,  0.0f,
			 0.0f,  0.0f,  1.0f,  0.0f,
			 0.0f, -1.0f,  0.0f,  0.0f,
			 0.0f,  0.0f,  0.0f,  1.0f		
		};

		const Matrix m_togl = aToYUp ? glm::make_mat4(&v_togl[0]) : Matrix(1.0f);

		Assimp::Importer importer;
		// put my custom IO handling in place
		const aiScene* scene = importer.ReadFile(aFileName,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);

		if (nullptr == scene) {
			Error(importer.GetErrorString());
			return false;
		}

		const int numMeshes = scene->mNumMeshes;
		for (int i = 0; i < numMeshes; i++)
		{
			aiMesh* src = scene->mMeshes[i];
			Mesh3d* dst = new Mesh3d(src->mName.C_Str());

			int numVerts = src->mNumVertices;
			int numFaces = src->mNumFaces;

			for (int v = 0; v < numVerts; v++)
			{
				
				VertexData vtx;
				Vector4f tmp(src->mVertices[v].x, src->mVertices[v].y, src->mVertices[v].z, 1.0f);
				Vector4f nor(src->mNormals[v].x, src->mNormals[v].y, src->mNormals[v].z, 0.0f);
				Vector4f trn = m_togl * tmp;
				Vector4f nrn = m_togl * nor;
				vtx.position = trn;
				vtx.normal = nrn;
				vtx.SetTexCoord(src->mTextureCoords[0][v].x, src->mTextureCoords[0][v].y);
				vtx.SetTangent(src->mTangents[v].x, src->mTangents[v].y, src->mTangents[v].y);
				vtx.SetBiTangent(src->mBitangents[v].x, src->mBitangents[v].y, src->mBitangents[v].z);

				dst->AddVertex(vtx);
			}

			for (int j = 0; j < numFaces; j++)
			{
				if (src->mFaces[j].mNumIndices != 3) continue;

				for (int k = 0; k < src->mFaces[j].mNumIndices; k++)
				{
					dst->AddIndex(src->mFaces[j].mIndices[k]);
				}
			}

			// Material

			aiColor3D color;
			float f;
			Material m;
			m.type = MaterialType_Specular;

			aiMaterial* material = scene->mMaterials[src->mMaterialIndex];

#if 0
			for (int pi = 0; pi < material->mNumProperties; pi++)
			{
				aiMaterialProperty* p = material->mProperties[pi];
				Info("%s[%s]", material->GetName().C_Str(), p->mKey.C_Str());
			}
#endif
			if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
			{
				m.diffuse = Vector3f(color.r, color.g, color.b);
				Info("%s: diffuse = %f,%f,%f", material->GetName().C_Str(), color.r, color.g, color.b);
			}
			if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
			{
				m.specular = Vector3f(color.r, color.g, color.b);
				Info("%s: specular = %f,%f,%f", material->GetName().C_Str(), color.r, color.g, color.b);
			}
			else 
			{
				m.specular = vec3(0.2f);
			}
			if (material->Get(AI_MATKEY_SHININESS, f) == AI_SUCCESS)
			{
				m.specularIntesity = f > 0.0001f ? f : 1.0f;
				Info("%s: shininess = %f", material->GetName().C_Str(), m.specularIntesity);
			}
			if (material->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, f) == AI_SUCCESS)
			{
				//m.specularIntesity = f > 0.0001f ? f : 1.0f;
				Info("%s: roughness = %f", material->GetName().C_Str(), f);
			}

			dst->SetMaterial(m);

			mMeshes.push_back(dst);
		}


		processNodesRecursive(scene->mRootNode, &mRootNode, Matrix(1.0f));

		/*
		* processing lights
		*/

		for (int i = 0; i < scene->mNumLights; i++)
		{
			const aiLight* light = scene->mLights[i];

			if (light->mType != aiLightSource_POINT)
			{
				continue;
			}

			aiMatrix4x4 rmat;
			aiNode* node = FindAINodeByName(light->mName, scene->mRootNode, rmat);

			if (node != NULL)
			{
				//const Matrix P = mat4_cast(node->mParent->mTransformation);
				const Matrix P = m_togl* mat4_cast(rmat) ;

				const Vector3f lpos = P * Vector4f(vec3_cast(light->mPosition), 1.0f);
				const Vector3f ldiff = color3_cast(light->mColorDiffuse);
				const Vector3f lspec = color3_cast(light->mColorSpecular);
				// Fatt = 1 / (1 + 2/r * d + 1/r2 * d2)
				PointLight* p = new PointLight(light->mName.C_Str(), lpos, ldiff, lspec);

				p->SetAttenuation(1.0f, 2.0f/mDefaultLightRadius, 1.0f/(mDefaultLightRadius2));

				Info("light %s  diffuse color: %.2f %.2f %.2f", p->GetName().c_str(), ldiff.r, ldiff.g, ldiff.b);
				Info("Light %s attenuation: %.2f %.2f %.2f", p->GetName().c_str(), p->GetConstantAtt(), p->GetLinearAtt(), p->GetQuadraticAtt());
				Info("Light %s radius: %.2f", p->GetName().c_str(), mDefaultLightRadius);

				mLights.push_back(p);

			}
		}

		if (scene->HasAnimations())
		{
			Info("Has animation: %d", scene->mNumAnimations);
			for (int a = 0; a < scene->mNumAnimations; a++)
			{
				aiAnimation* e = scene->mAnimations[a];
				Info("name: %s, dur: %.2f, chan: %d, mesh.chan: %d, ticks/sec: %.2f", e->mName.C_Str(), e->mDuration, e->mNumChannels, e->mNumMeshChannels, e->mTicksPerSecond);

				for (int i = 0; i < e->mNumChannels; i++)
				{
					aiNodeAnim* anim = e->mChannels[i];
					auto search = mNodeByName.find(anim->mNodeName.C_Str());
					Node3d* myNode = search != mNodeByName.end() ? search->second : nullptr;

					if (myNode == nullptr) continue;

					Animation* myAnim = myNode->CreateAnimation(e->mName.C_Str());
					myAnim->SetTicksPerSec(e->mTicksPerSecond);
					myAnim->SetLength(e->mDuration);
					myAnim->SetNode(myNode);

					Info("channel %d, nodeName: %s, pos-keys: %d, rot-keys: %d", i, anim->mNodeName.C_Str(), anim->mNumPositionKeys, anim->mNumRotationKeys);
					if (anim->mNumPositionKeys > 0)
					{
						AnimationTrack* track = myAnim->CreateTrack("Position");

						for (int k = 0; k < anim->mNumPositionKeys; k++)
						{
							aiVectorKey p_key = anim->mPositionKeys[k];
							
							//Info("Key %d, Time: %.2f, Pos:(%.2f, %.2f, %.2f)", k, p_key.mTime,
							//	p_key.mValue.x,
							//	p_key.mValue.y,
							//	p_key.mValue.z);

							Keyframe* kf = track->CreateKeyframe(p_key.mTime);
							kf->position = vec3_cast(p_key.mValue);
							kf->rotation = Quat(1, 0, 0, 0);
						}
					}

					if (anim->mNumRotationKeys > 0)
					{
						AnimationTrack* track = myAnim->CreateTrack("Rotation");
						for (int k = 0; k < anim->mNumRotationKeys; k++)
						{
							aiQuatKey q_key = anim->mRotationKeys[k];

							//Info("Key %d, Time: %.2f, Q:(%.2f, %.2f, %.2f, %.2f)", k, q_key.mTime,
							//	q_key.mValue.w,
							//	q_key.mValue.x,
							//	q_key.mValue.y,
							//	q_key.mValue.z);

							Keyframe* kf = track->CreateKeyframe(q_key.mTime);
							kf->position = Vector3f(0.0f);
							kf->rotation = quat_cast(q_key.mValue);
						}
					}
				}
			}
		}

		return true;
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

		const unsigned int kVertexSize = sizeof(VertexData);

		assert((kVertexSize & 15) == 0);

		size_t l_requiredVertexBufferSize = 0;
		size_t l_requiredIndexBufferSize = 0;

		for (int i = 0; i < mMeshes.size(); i++)
		{
			l_requiredVertexBufferSize += (mMeshes[i]->vertices.size() * kVertexSize);
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
			const Mesh3d* m = mMeshes[i];
			FlatBufferHandle_t vtxH, idxH;

			vtxH.size = m->vertices.size() * kVertexSize;
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
		vAttr.AddVertexAttrib(VertexBufferElement_Position, VtxAttribType_Float, 3, kVertexSize, 0, vb);
		vAttr.AddVertexAttrib(VertexBufferElement_Normal, VtxAttribType_Float, 3, kVertexSize, 0, vb);
		vAttr.AddVertexAttrib(VertexBufferElement_Tangent, VtxAttribType_Float, 3, kVertexSize, 0, vb);
		vAttr.AddVertexAttrib(VertexBufferElement_BiTangent, VtxAttribType_Float, 3, kVertexSize, 0, vb);
		vAttr.AddVertexAttrib(VertexBufferElement_Texture0, VtxAttribType_Float, 2, kVertexSize, 0, vb);
		mVA = mGd->CreateVertexArray(ib, vAttr);

		mVA->Compile();
		mCompiled = true;

		return true;
	}

	void Scene::Draw(GpuShader* aShader)
	{

		mDrawList.clear();
		BuildDrawList(&mRootNode);
		std::sort(mDrawList.begin(), mDrawList.end(), Scene_MeshOrderComparator);

		/************************************
		Render Z-Pass
		*************************************/

		mGd->SetsRGBFrameBufferEnabled(true);

		mVA->Bind();

		mGd->SetCullFaceEnable(true);

		mRPass = RenderPass_Z;
		mGd->SetDepthMask(true);
		mGd->SetDepthTestEnable(true);
		mGd->SetDepthTestFunc(DepthTestFunc_Less);
		mGd->SetClearDepth(1.0f);
		mGd->ClearFrameBuffer(ClearFBFlags_Depth);
		mGd->SetColorMask(ColorMask_None);
		mGd->SetBlendEnabled(false);

		DrawList();

		/************************************
		Render Ambient
		*************************************/

		mGd->SetDepthMask(false);
		mGd->SetDepthTestFunc(DepthTestFunc_LessOrEqual);
		mGd->SetColorMask(ColorMask_RGBA);
		mGd->ClearFrameBuffer(ClearFBFlags_Color);

		mRPass = RenderPass_Ambient;

		DrawList();

		/************************************
		Render Lights
		*************************************/
		mGd->SetBlendEnabled(true);
		mGd->SetBlendFunc(BlendFunc_One, BlendFunc_One);
		mGd->SetDepthTestFunc(DepthTestFunc_Equal);
		mRPass = RenderPass_Light;

		for (auto it = mLights.begin(); it != mLights.end(); it++)
		{
			if ((*it)->GetType() == LightType_Point)
			{
				mCurrentLight = *it;
				DrawList();
			}

		}

		mGd->UseShader(NULL);
		mGd->SwapBuffers();

	}

	float Scene::SetDefaultLightRadius(const float a0)
	{
		float prev = mDefaultLightRadius;
		mDefaultLightRadius = a0;
		mDefaultLightRadius2 = a0 * a0;

		Info("Light attenuation: %.2f %.2f %.2f", 1.0f, 2.0f / mDefaultLightRadius, 1.0f / mDefaultLightRadius2);

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
		return Scene_recurse_search(&mRootNode, aName);
	}

	MeshQueryResult Scene::GetMeshByName(const String& aName)
	{
		for (int i = 0; i < mMeshes.size(); i++)
		{
			if (mMeshes[i]->mName == aName)
				return MeshQueryResult(mMeshes[i], i);
		}

		return MeshQueryResult(nullptr, -1);
	}

	void Scene::processNodesRecursive(const aiNode* node, Node3d* aParent, Matrix accTransform)
	{
		Matrix transform(1.0f);
		Node3d* parent;
		if (node->mNumMeshes > 0)
		{
			String name = node->mName.C_Str();
			std::vector<String> nameVec = split(name, "_");
			bool visible = true;

			for (int i = 1; i < nameVec.size(); i++)
			{
				if (nameVec[i] == "i")
					visible = false;
			}
			
			// create node
			Node3d* nNode = new Node3d(nameVec[0]);
			mNodeByName.insert(tNodeByNamePair(name, nNode));

			const Matrix M = mat4_cast(node->mTransformation);
			nNode->SetWorldTransform(M * accTransform);
			nNode->SetTransform(M, false);
			nNode->SetVisible(visible);

			aParent->AddChildNode(nNode);
			for (int k = 0; k < node->mNumMeshes; k++)
			{
				nNode->AddMesh(node->mMeshes[k]);
			}

			parent = nNode;
			transform = Matrix(1.0f);

		}
		else
		{
			parent = aParent;
			transform = mat4_cast(node->mTransformation) * accTransform;
		}

		for (int k = 0; k < node->mNumChildren; k++)
		{
			processNodesRecursive(node->mChildren[k], parent, transform);
		}

	}

	void Scene::BuildDrawList(Node3d* node)
	{
		if (!node->IsVisible())
			return;

		node->UpdateWorldTransform(false);

		if (node->HasChildren())
		{
			const Node3dPtrVec& children = node->GetChildren();
			for (auto it = children.begin(); it != children.end(); it++)
			{
				BuildDrawList(*it);
			}
		}


		const MeshIdxVec& list = node->GetMeshes();
		const Matrix M = node->GetWorldTransform();
		const Matrix NM = Matrix3x3(glm::transpose(glm::inverse(M)));
		const Matrix MVP = mP * mV * M;

		for (auto it = list.begin(); it != list.end(); it++)
		{
			const int idx = *it;
			const Mesh3d* mesh = mMeshes[idx];
			DrawEntityDef_t entity(idx, mesh->mMaterial.type, NM, M, MVP);

			mDrawList.push_back(entity);
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
			mCurrentShader->SetVector3("viewPos", &mViewPos[0]);
			mCurrentShader->Use();
		}

		for (auto it = mDrawList.begin(); it != mDrawList.end(); it++)
		{
			DrawEntityDef_t ent = *it;
			
			if (mtCurrent != ent.mMaterial && mtCurrent != MaterialType_ZPass)
			{
				mtCurrent = ent.mMaterial;
				mCurrentShader = mSm->GetShaderByMaterial(mtCurrent);
				mCurrentShader->Use();
				mCurrentShader->SetVector3("viewPos", &mViewPos[0]);

				if (mRPass == RenderPass_Ambient)
				{
					mCurrentShader->SetVector3("light.position", &cBlack[0]);
					mCurrentShader->SetVector3("light.diffuse", &cBlack[0]);
					mCurrentShader->SetVector3("light.specular", &cBlack[0]);
				}
				else if (mRPass == RenderPass_Light)
				{
					PointLight* p = reinterpret_cast<PointLight*> (mCurrentLight);
					p->SetAttenuation(1.0f, 2.0f / mDefaultLightRadius, 1.0f / (mDefaultLightRadius2));

					mCurrentShader->SetVector3("light.position", &(p->GetPosition())[0]);
					mCurrentShader->SetVector3("light.diffuse", &(p->GetDiffuse())[0]);
					mCurrentShader->SetVector3("light.specular", &(p->GetSpecular())[0]);
					mCurrentShader->SetFloat("light.kc", p->GetConstantAtt());
					mCurrentShader->SetFloat("light.kl", p->GetLinearAtt());
					mCurrentShader->SetFloat("light.kq", p->GetQuadraticAtt());
				}
			}

			mCurrentShader->SetMatrix("M", &ent.mModelTrans[0][0]);
			mCurrentShader->SetMatrix("NM", &ent.mNormalTrans[0][0]);
			mCurrentShader->SetMatrix("MVP", &ent.mMVP[0][0]);


			DrawMesh(ent.mMeshIndex);
		}
	}

	void Scene::DrawMesh(const unsigned aMesh) const
	{
		const Mesh3d* m = mMeshes[aMesh];
		const Vector3f kBlack(0.0f);

		FlatBufferHandle_t vtxH = mVertexBufferHandles[aMesh];
		FlatBufferHandle_t idxH = mIndexBufferHandles[aMesh];
		size_t baseVert = vtxH.offset / sizeof(VertexData);

		if (mRPass == RenderPass_Light)
		{
			mCurrentShader->SetVector3("material.ambient", &kBlack[0]);
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

		mGd->DrawPrimitivesWithBase(PrimitiveType_Triangles, m->indices.size(), idxH.offset, baseVert);
	}

	void Scene::Init()
	{
	}

	aiNode* Scene::FindAINodeByName(const aiString& aName, aiNode* aNode, aiMatrix4x4& aTrans)
	{

		if (aNode->mName == aName)
		{
			aiMatrix4x4 m = aNode->mTransformation;
			for (aiNode* parent = aNode->mParent; parent != NULL; parent = parent->mParent)
			{
				m = parent->mTransformation * m;
			}

			aTrans = m;
			return aNode;
		}

		if (aNode->mNumChildren > 0)
		{
			for (int i = 0; i < aNode->mNumChildren; i++)
			{
				aiNode* xnode = FindAINodeByName(aName, aNode->mChildren[i], aTrans);
				if (xnode != NULL) {
					aTrans = aTrans;
					return xnode;
				}
			}
		}

		return NULL;
	}

}
