#include "system/Strings.hpp"
#include "scene/GltfLoader.hpp"
#include "scene/Mesh3d.hpp"
#include "scene/Scene.hpp"
#include <tiny_gltf.h>

#include <algorithm>
#include <cassert>
#include <glm/gtc/type_ptr.hpp>

namespace jse {
	
    template<class T>
    static const T* GltfLoader_cast_array(uint8_t const* byte_ptr)
    {
        return reinterpret_cast<T const*>(byte_ptr);
    }

	template<typename T, typename Func>
	static void GltfLoader_Extract_scalar(const T * aPtr, const size_t aCount, const size_t aStride, const Func& aCB)
	{
		size_t idx = 0;
		for (size_t i = 0; i < aCount; i++)
		{
			aCB(aPtr[idx]);
			idx += aStride;
		}
	}

	int GltfLoader::LoadScene(const String& aFilename)
	{
		bool isAscii = true;
		bool ret = false;
		String err;
		String warn;
		std::map<int, Mesh3d*> meshMap;

		if (aFilename.find(".glb") != String::npos)
			isAscii = false;

		if (isAscii)
		{
			ret = mLoader.LoadASCIIFromFile(&mModel, &err, &warn, aFilename);
		}
		else
		{
			ret = mLoader.LoadBinaryFromFile(&mModel, &err, &warn, aFilename);
		}

		if (!warn.empty()) {
			Warning("Warn: %s", warn.c_str());
		}

		if (!err.empty()) {
			Error("Err: %s", err.c_str());
		}

		if (!ret) {
			Error("Failed to parse glTF [%s]", aFilename.c_str());
			return -1;
		}

		tinygltf::Scene& scene = mModel.scenes[mModel.defaultScene];

		Info("Number of meshes: %d", mModel.meshes.size());

		for (size_t i = 0; i < mModel.meshes.size(); i++)
		{
			const tinygltf::Mesh& m = mModel.meshes[i];
			Mesh3d* dst = new Mesh3d(m.name);
			meshMap.insert(std::make_pair(int(i), dst));

			for (size_t j = 0; j < m.primitives.size(); j++)
			{
				tinygltf::Primitive p = m.primitives[j];
				tinygltf::Accessor indexAccessor = mModel.accessors[p.indices];

                tinygltf::BufferView& ibv = mModel.bufferViews[indexAccessor.bufferView];
                tinygltf::Buffer& ibuf = mModel.buffers[ibv.buffer];

				if (p.mode != TINYGLTF_MODE_TRIANGLES)
				{
					Warning("GLTF-WARN: Only TRIANGLES mode is supported!");
					return -1;
				}

                if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                {
                    auto const* iIn = GltfLoader_cast_array<uint16_t>(ibuf.data.data() + indexAccessor.byteOffset + ibv.byteOffset);
					GltfLoader_Extract_scalar(iIn, indexAccessor.count, indexAccessor.ByteStride(ibv) / sizeof(uint16_t), [dst](uint16_t const val) {dst->AddIndex(val); });
                }
				else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
				{
					auto const* iIn = GltfLoader_cast_array<uint32_t>(ibuf.data.data() + indexAccessor.byteOffset + ibv.byteOffset);
					GltfLoader_Extract_scalar(iIn, indexAccessor.count, indexAccessor.ByteStride(ibv) / sizeof(uint32_t), [dst](uint32_t const val) {dst->AddIndex(val); });
				}
                
				const float* v_pos, * v_tex, * v_norm, * v_tan;
				v_pos = v_tex = v_norm = v_tan = nullptr;

				size_t numPrimitives = 0;

				for (auto& attrib : p.attributes)
				{
					tinygltf::Accessor access = mModel.accessors[attrib.second];
					tinygltf::BufferView& bv = mModel.bufferViews[access.bufferView];
					tinygltf::Buffer& buf = mModel.buffers[bv.buffer];

					
                    if (access.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
                    {
                        Error("Only float component type implemented !");
                        return -1;
                    }
                    
                    //const size_t byteStride = access.ByteStride(bv) / sizeof(float);

                    const uint8_t* buffer = buf.data.data() + access.byteOffset + bv.byteOffset;
					const float* vIn = GltfLoader_cast_array<float>(buffer);
                    
                    size_t offset = 0;
					if (attrib.first.compare("POSITION") == 0)
					{
						v_pos = vIn;
					}
					else if (attrib.first.compare("NORMAL") == 0)
					{
						v_norm = vIn;
					}
					else if (attrib.first.compare("TEXCOORD_0") == 0)
					{
						v_tex = vIn;
					}
					else if (attrib.first.compare("TANGENT") == 0)
					{
						v_tan = vIn;
					}

					numPrimitives = access.count;
				}

				dst->SetData(v_pos, v_norm, v_tan, v_tex, numPrimitives);
				dst->CompileFromData();
				mScene.mMeshes.push_back(dst);

				if (p.material > -1)
				{
					tinygltf::Material mat = mModel.materials[p.material];
					Material xm;
					xm.type = MaterialType_Specular;
					xm.diffuse = Color3(mat.pbrMetallicRoughness.baseColorFactor[0], mat.pbrMetallicRoughness.baseColorFactor[1], mat.pbrMetallicRoughness.baseColorFactor[2]);
					xm.specular = Color3(0.2f);
					xm.specularIntesity = 50.f;
					xm.ambient = Color3(.1f);
					dst->SetMaterial(xm);
				}
				
			}

		}

		for (size_t i : scene.nodes)
		{
			tinygltf::Node& xnode = mModel.nodes[i];
			if (xnode.mesh > -1)
			{
				Info("Node %s, %d", xnode.name.c_str(), i);
				RecursiveProcessNode(xnode, &mScene.mRootNode, Matrix(1.0f), 0);
			}
		}
		
		for (int i = 0; i < mModel.lights.size(); i++)
		{
			tinygltf::Light& light = mModel.lights[i];
			Vector3f lpos = vec3(0.f);

			if (light.type == "point")
			{
				for (auto n : scene.nodes)
				{
					const tinygltf::Node& node = mModel.nodes[n];
					if (node.name == light.name) {
						lpos = vec3(node.translation[0], node.translation[1], node.translation[2]);
					}
				}
				const Vector3f ldiff = Color3(light.color[0], light.color[1], light.color[2]) * float(light.intensity);
				const Vector3f lspec = ldiff;;
				// Fatt = 1 / (1 + 2/r * d + 1/r2 * d2)
				PointLight* p = new PointLight(light.name, lpos, ldiff, lspec, 2.0f / mScene.mDefaultLightRadius, 1.0f / mScene.mDefaultLightRadius2, 0.0001f);
				mScene.mLights.push_back(p);

			}
		}

		return 0;
	}

	void GltfLoader::RecursiveProcessNode(const tinygltf::Node& aNode, Node3d* aParent, const Matrix& accTransform, int level)
	{
		Matrix transform(1.0f);
		Matrix M(1.0f);
		Node3d* parent;
		float mtmp[16];

		if (aNode.matrix.size() == 16)
		{
			for (int i = 0; i < 16; i++)
			{
				mtmp[i] = float(aNode.matrix[i]);
			}
			M = glm::make_mat4(mtmp);
		}
		else {

			if (aNode.translation.size() == 3)
			{
				M = glm::translate(M, vec3(float(aNode.translation[0]), float(aNode.translation[1]), float(aNode.translation[2])));
			}
			if (aNode.scale.size() == 3)
			{
				M = glm::scale(M, vec3(float(aNode.scale[0]), float(aNode.scale[1]), float(aNode.scale[2])));
			}
			if (aNode.rotation.size() == 4)
			{
				const Quat q(float(aNode.rotation[3]), float(aNode.rotation[0]), float(aNode.rotation[1]), float(aNode.rotation[2]));
				M = M * glm::mat4_cast(q);
			}
		}

		String name = aNode.name;
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


		nNode->SetWorldTransform(M * accTransform);
		nNode->SetTransform(M, false);
		nNode->SetVisible(visible);
		aParent->AddChildNode(nNode);

		nNode->AddMesh(aNode.mesh);

		parent = nNode;
		transform = M * accTransform;

		char lc[200];

		memset(lc, '-', 2*level);
		lc[2*level] = '\0';
//		Info("%s Node %s", lc, name.c_str());

		for (size_t i : aNode.children)
		{
			tinygltf::Node& xnode = mModel.nodes[i];
			if (xnode.mesh > -1)
			{
				Info("%s Child %s, %d", lc, xnode.name.c_str(), i);

				RecursiveProcessNode(xnode, parent, transform, level + 1);
			}
		}
	}

}
