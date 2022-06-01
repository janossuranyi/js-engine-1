#include "scene/AssimpLoader.hpp"

#include "scene/Scene.hpp"
#include "scene/Mesh3d.hpp"
#include "scene/Node3d.hpp"
#include "scene/Animation.hpp"
#include "scene/AnimationTrack.hpp"
#include "system/Logger.hpp"
#include "system/Strings.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace jse {

	static inline Vector3f color3_cast(const aiColor3D& v) { return Vector3f(v.r, v.g, v.b); }
	static inline Vector3f vec3_cast(const aiVector3D& v) { return Vector3f(v.x, v.y, v.z); }
	static inline Quat quat_cast(const aiQuaternion& q) { return Quat(q.w, q.x, q.y, q.z); }
	static inline Vector2f vec2_cast(const aiVector3D& v) { return Vector2f(v.x, v.y); } // it's aiVector3D because assimp's texture coordinates use that
	static inline Matrix mat4_cast(const aiMatrix4x4& m) { return glm::transpose(glm::make_mat4(&m.a1)); }

	void AssimpLoader::processNodesRecursive(const aiNode* node, Node3d* aParent)
	{
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
			mScene.mNodeByName.insert(Scene::tNodeByNamePair(name, nNode));

			const Matrix M = mat4_cast(node->mTransformation);
			nNode->SetTransform(M, true);
			nNode->SetVisible(visible);

			aParent->AddChildNode(nNode);
			for (int k = 0; k < node->mNumMeshes; k++)
			{
				nNode->AddRenderable(mScene.GetMeshByIndex(node->mMeshes[k]));
			}

			parent = nNode;
		}
		else
		{
			parent = aParent;
		}

		for (int k = 0; k < node->mNumChildren; k++)
		{
			processNodesRecursive(node->mChildren[k], parent);
		}

	}

    int AssimpLoader::LoadScene(const String& aFileName)
    {
		
		const float v_togl[16] = {
			 1.0f,  0.0f,  0.0f,  0.0f,
			 0.0f,  0.0f,  1.0f,  0.0f,
			 0.0f, -1.0f,  0.0f,  0.0f,
			 0.0f,  0.0f,  0.0f,  1.0f
		};

		const Matrix m_togl = Matrix(1.0f);

		Assimp::Importer importer;
		// put my custom IO handling in place
		const aiScene* scene = importer.ReadFile(aFileName,
			aiProcess_CalcTangentSpace |
			aiProcess_GenNormals |
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);

		if (nullptr == scene) {
			Error(importer.GetErrorString());
			return false;
		}

		const int numMeshes = scene->mNumMeshes;
		Info("Number of meshes: %d", numMeshes);
		Info("Number of textures: %d", scene->mNumTextures);

		for (int i = 0; i < numMeshes; i++)
		{
			aiMesh* src = scene->mMeshes[i];
			Mesh3d dst(src->mName.C_Str());

			const auto numVerts = src->mNumVertices;
			const auto numFaces = src->mNumFaces;

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

				dst.AddVertex(vtx);
			}

			for (int j = 0; j < numFaces; j++)
			{
				if (src->mFaces[j].mNumIndices != 3) continue;

				for (int k = 0; k < src->mFaces[j].mNumIndices; k++)
				{
					dst.AddIndex(src->mFaces[j].mIndices[k]);
				}
			}

			// Material

			aiColor3D color;
			float f;
			Material m;
			m.type = MaterialType_Specular;

			aiMaterial* material = scene->mMaterials[src->mMaterialIndex];
			for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
			{
				aiString str;
				material->GetTexture(aiTextureType_DIFFUSE, i, &str);
				Info("Diffuse-Texture: %s", str.C_Str());
			}
			for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS); i++)
			{
				aiString str;
				material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, i, &str);
				Info("Roughness-Texture: %s", str.C_Str());
			}
			for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_NORMALS); i++)
			{
				aiString str;
				material->GetTexture(aiTextureType_NORMALS, i, &str);
				Info("Normals-Texture: %s", str.C_Str());
			}
			for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_METALNESS); i++)
			{
				aiString str;
				material->GetTexture(aiTextureType_METALNESS, i, &str);
				Info("METALNESS-Texture: %s", str.C_Str());
			}


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
			dst.SetMaterial(m);

			mScene.AddMesh(dst);
		}


		processNodesRecursive(scene->mRootNode, &mScene.mRootNode);

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
			aiNode* node = mScene.FindAINodeByName(light->mName, scene->mRootNode, rmat);

			if (node != NULL)
			{
				//const Matrix P = mat4_cast(node->mParent->mTransformation);
				const Matrix P = m_togl * mat4_cast(rmat);

				const Vector3f lpos = P * Vector4f(vec3_cast(light->mPosition), 1.0f);
				const Vector3f ldiff = color3_cast(light->mColorDiffuse);
				const Vector3f lspec = color3_cast(light->mColorSpecular);
				// Fatt = 1 / (1 + 2/r * d + 1/r2 * d2)
				auto p = std::make_shared<PointLight>(light->mName.C_Str(), lpos, ldiff, lspec, 2.0f / mScene.mDefaultLightRadius, 1.0f / mScene.mDefaultLightRadius2, 0.0001);

				Info("light %s  diffuse color: %.2f %.2f %.2f", p->GetName().c_str(), ldiff.r, ldiff.g, ldiff.b);
				Info("Light %s attenuation: %.2f %.2f %.2f", p->GetName().c_str(), p->GetConstantAtt(), p->GetLinearAtt(), p->GetQuadraticAtt());
				Info("Light %s radius: %.2f", p->GetName().c_str(), mScene.mDefaultLightRadius);

				Node3d* nNode = new Node3d(p->GetName());
				nNode->AddRenderable(p);
				nNode->SetPosition(lpos);

				mScene.AddNode(nNode);

			}
		}

		if (scene->HasAnimations())
		{
			Info("Has animation: %d", scene->mNumAnimations);
			for (int a = 0; a < scene->mNumAnimations; a++)
			{
				aiAnimation* e = scene->mAnimations[a];
				Info("name: %s, dur: %.2f, chan: %d, mesh.chan: %d, ticks/sec: %.2f", e->mName.C_Str(), e->mDuration, e->mNumChannels, e->mNumMeshChannels, e->mTicksPerSecond);

				Animation* myAnim = new Animation(e->mName.C_Str());
				myAnim->SetTicksPerSec(float(e->mTicksPerSecond));
				myAnim->SetLength(float(e->mDuration));
				mScene.mAnimMgr.AddAnimation(myAnim);

				for (int i = 0; i < e->mNumChannels; i++)
				{
					aiNodeAnim* anim = e->mChannels[i];
					auto search = mScene.mNodeByName.find(anim->mNodeName.C_Str());
					Node3d* myNode = search != mScene.mNodeByName.end() ? search->second : nullptr;

					if (myNode == nullptr) continue;
					//myNode->SetAnimated(true);


					Info("channel %d, nodeName: %s, pos-keys: %d, rot-keys: %d", i, anim->mNodeName.C_Str(), anim->mNumPositionKeys, anim->mNumRotationKeys);
					if (anim->mNumPositionKeys > 0)
					{
						AnimationTrack& track = myAnim->CreateTrack(anim->mNodeName.C_Str(), AnimationTrackType_Position, myNode);

						for (int k = 0; k < anim->mNumPositionKeys; k++)
						{
							aiVectorKey p_key = anim->mPositionKeys[k];

							//Info("Key %d, Time: %.2f, Pos:(%.2f, %.2f, %.2f)", k, p_key.mTime,
							//	p_key.mValue.x,
							//	p_key.mValue.y,
							//	p_key.mValue.z);

							Keyframe& kf = track.CreateKeyframe(float(p_key.mTime));
							kf.v = vec3_cast(p_key.mValue);
						}
					}

					if (anim->mNumRotationKeys > 0)
					{
						AnimationTrack& track = myAnim->CreateTrack(anim->mNodeName.C_Str(), AnimationTrackType_Rotation, myNode);

						for (int k = 0; k < anim->mNumRotationKeys; k++)
						{
							aiQuatKey q_key = anim->mRotationKeys[k];

							//Info("Key %d, Time: %.2f, Q:(%.2f, %.2f, %.2f, %.2f)", k, q_key.mTime,
							//	q_key.mValue.w,
							//	q_key.mValue.x,
							//	q_key.mValue.y,
							//	q_key.mValue.z);

							Keyframe& kf = track.CreateKeyframe(q_key.mTime);
							kf.q = quat_cast(q_key.mValue);
						}
					}
				}
			}
		}
	}

}
