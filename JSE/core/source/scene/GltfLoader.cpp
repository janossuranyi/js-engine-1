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
	static void GltfLoader_Extract_scalar(const T* aPtr, const size_t aCount, const size_t aStride, const Func& aCB)
	{
		size_t idx = 0;
		for (size_t i = 0; i < aCount; i++)
		{
			aCB(aPtr[idx]);
			idx += aStride;
		}
	}

	static unsigned int GltfLoader_GetComponentSize(int aType)
	{
		unsigned size = 1;
		switch (aType)
		{
		case TINYGLTF_COMPONENT_TYPE_SHORT:
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			size = 2;
			break;
		case TINYGLTF_COMPONENT_TYPE_INT:
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			size = 4;
			break;
		case TINYGLTF_COMPONENT_TYPE_DOUBLE:
			size = 8;
			break;
		default:
			break;
		}

		return size;
	}

	template<class T>
	void GltfLoader::ExtractData(const tinygltf::Accessor& aAccessor, std::unique_ptr<T>& aDest)
	{
		const tinygltf::BufferView& view = mModel.bufferViews[aAccessor.bufferView];
		const tinygltf::Buffer& buf = mModel.buffers[view.buffer];

		unsigned size = GltfLoader_GetComponentSize(aAccessor.componentType);

		const size_t elemCount = aAccessor.count;
		const unsigned elemSize = size * aAccessor.type;
		const unsigned stride = aAccessor.ByteStride(view);
		const unsigned targetElemSize = sizeof(T);
		const size_t totalSize = size * aAccessor.type * elemCount;
		const uint8_t* data = buf.data.data() + aAccessor.byteOffset + view.byteOffset;

		T* tmp = new T[aAccessor.count];
		aDest = std::unique_ptr<T>(tmp);

		if (stride == elemSize && targetElemSize == elemSize)
		{
			std::memcpy(tmp, data, totalSize);
		}
		else {
			for (unsigned i = 0; i < aAccessor.count; ++i)
			{
				std::memcpy(tmp + i, data + stride * i, elemSize);
			}
		}
	}

	int GltfLoader::LoadScene(const String& aFilename)
	{
		bool isAscii = true;
		bool ret = false;
		String err;
		String warn;


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

		meshOffsets.clear();
		unsigned int k = 0;

		for (unsigned int i = 0; i < mModel.meshes.size(); ++i)
		{
			const tinygltf::Mesh& m = mModel.meshes[i];
			meshOffsets.push_back(k);
			k += unsigned(m.primitives.size());

			for (unsigned int j = 0; j < m.primitives.size(); j++)
			{
				Mesh3d dst(m.name);

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
					GltfLoader_Extract_scalar(iIn, indexAccessor.count, indexAccessor.ByteStride(ibv) / sizeof(uint16_t), [&dst](uint16_t const val) {dst.AddIndex(val); });
				}
				else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
				{
					auto const* iIn = GltfLoader_cast_array<uint32_t>(ibuf.data.data() + indexAccessor.byteOffset + ibv.byteOffset);
					GltfLoader_Extract_scalar(iIn, indexAccessor.count, indexAccessor.ByteStride(ibv) / sizeof(uint32_t), [&dst](uint32_t const val) {dst.AddIndex(val); });
				}



				auto findPos = p.attributes.find("POSITION");
				if (findPos == p.attributes.end())
				{
					Warning("GLTF-WARN: POSITION attributes missing !");
					return -1;
				}

				/* Copy position data */

				tinygltf::Accessor& posAccessor = mModel.accessors[findPos->second];
				const size_t numPrimitives = posAccessor.count;

				std::unique_ptr<vec3> v_pos;
				std::unique_ptr<vec3> v_norm;
				std::unique_ptr<vec4> v_tan;
				std::unique_ptr<vec2> v_tex;

				ExtractData(posAccessor, v_pos);

				{
					auto f = p.attributes.find("NORMAL");
					if (f != p.attributes.end())
					{
						ExtractData(mModel.accessors[f->second], v_norm);
					}
				}
				{
					auto f = p.attributes.find("TEXCOORD_0");
					if (f != p.attributes.end())
					{
						ExtractData(mModel.accessors[f->second], v_tex);
					}
				}
				{
					auto f = p.attributes.find("TANGENT");
					if (f != p.attributes.end())
					{
						ExtractData(mModel.accessors[f->second], v_tan);
					}
				}

				dst.SetData(v_pos.get(), v_norm.get(), v_tan.get(), v_tex.get(), numPrimitives);
				dst.CompileFromData();

				if (p.material > -1)
				{
					tinygltf::Material mat = mModel.materials[p.material];
					Material xm;
					xm.type = MaterialType_Specular;
					xm.diffuse = Color3(mat.pbrMetallicRoughness.baseColorFactor[0], mat.pbrMetallicRoughness.baseColorFactor[1], mat.pbrMetallicRoughness.baseColorFactor[2]);
					xm.specular = Color3(0.2f);
					xm.specularIntesity = float(mat.pbrMetallicRoughness.roughnessFactor * 100.0);
					xm.ambient = Color3(.1f);
					dst.SetMaterial(xm);
				}
				mScene.AddMesh(dst);
			}
		}
		meshOffsets.push_back(k);

		for (size_t i : scene.nodes)
		{
			tinygltf::Node& xnode = mModel.nodes[i];

			Node3d* nNode = ImportNode(xnode);
			if (nNode) 
			{
				mScene.AddNode(nNode, nullptr);
				nNode->UpdateWorldTransform();
			}
			
		}

		for (auto anim : mModel.animations)
		{
			Info("Animation: %s", anim.name.c_str());

			Animation* myAnim = new Animation(anim.name.c_str());
			myAnim->SetTicksPerSec(1000.f);
			myAnim->SetLength(0.0f);
			float length = 0.0f;

			for (auto channel : anim.channels)
			{
				const tinygltf::AnimationSampler& sampler = anim.samplers[channel.sampler];
				const tinygltf::Accessor& input = mModel.accessors[sampler.input];
				const tinygltf::Accessor& output = mModel.accessors[sampler.output];
				const tinygltf::Node& target = mModel.nodes[channel.target_node];

				Info("\\__ Channel: %s, target: %d, Interpolation: %s", channel.target_path.c_str(), channel.target_node, sampler.interpolation.c_str());
				
				//tinygltf::Node& node = mModel.nodes[channel.target_node];


				auto search = mScene.mNodeByName.find(target.name);
				Node3d* myNode = search != mScene.mNodeByName.end() ? search->second : nullptr;

				if (myNode == nullptr) continue;

				AnimationTrackType trackType = AnimationTrackType_LastEnum;

				if (channel.target_path == "translation")
				{
					trackType = AnimationTrackType_Position;
				}
				else if (channel.target_path == "rotation")
				{
					trackType = AnimationTrackType_Rotation;
				}
				else 
				{
					continue;
				}

				AnimationTrack& track = myAnim->CreateTrack(target.name, trackType, myNode);

				const tinygltf::BufferView inbv = mModel.bufferViews[input.bufferView];
				const tinygltf::Buffer inbuf = mModel.buffers[inbv.buffer];

				const float* timestamps = reinterpret_cast<float const*>(inbv.byteOffset + input.byteOffset + inbuf.data.data());

				const tinygltf::BufferView outbv = mModel.bufferViews[output.bufferView];
				const tinygltf::Buffer outbuf = mModel.buffers[outbv.buffer];

				const float* values = reinterpret_cast<float const*>(outbv.byteOffset + output.byteOffset + outbuf.data.data());

				for (unsigned i = 0; i < input.count; ++i)
				{
					Keyframe& kf = track.CreateKeyframe(timestamps[i] * 1000.f);

					if (trackType == AnimationTrackType_Position)
					{
						kf.v = glm::make_vec3(&values[i * 3]);
					}
					else 
					{
						kf.q = Quat(values[i * 4 + 3], values[i * 4 + 0], values[i * 4 + 1], values[i * 4 + 2]);
					}
				}
				if (length < timestamps[input.count - 1])
				{
					length = timestamps[input.count - 1];
				}
			}
			myAnim->SetLength(1000.f * length);
			mScene.mAnimMgr.AddAnimation(myAnim);
			
		}

		return 0;
	}

	Node3d* GltfLoader::ImportNode(const tinygltf::Node& aNode, unsigned aLevel)
	{
		String name = aNode.name;
		std::vector<String> nameVec = split(name, "_");
		bool visible = true;
		int light = -1;

		for (int i = 1; i < nameVec.size(); i++)
		{
			if (nameVec[i] == "i")
				visible = false;
		}

		char cbuf[200];
		std::memset(cbuf, 0, 200);
		unsigned cc = 0;
		while (cc <= aLevel*2)
		{
			cbuf[cc++] = '_';
		}

		if (aNode.mesh == -1 && !aNode.extensions.empty())
		{
			auto findLight = aNode.extensions.find("KHR_lights_punctual");
			if (findLight != aNode.extensions.end())
			{
				light = findLight->second.Get("light").GetNumberAsInt();
			}
		}

		// create node
		Node3d* nNode = new Node3d(name);
		nNode->SetVisible(visible);
		mScene.mNodeByName.insert(Scene::tNodeByNamePair(name, nNode));

		/* Get node transform */

		Matrix M(1.0f);
		float mtmp[16];

		if (aNode.matrix.size() == 16)
		{
			for (int i = 0; i < 16; i++)
			{
				mtmp[i] = float(aNode.matrix[i]);
			}
			nNode->SetTransform(glm::make_mat4(mtmp), true);
		}
		else {

			if (aNode.translation.size() == 3)
			{
				nNode->SetPosition(vec3(
					float(aNode.translation[0]),
					float(aNode.translation[1]),
					float(aNode.translation[2])));
			}
			if (aNode.rotation.size() == 4)
			{
				nNode->AddRotation(Quat(
					float(aNode.rotation[3]),
					float(aNode.rotation[0]),
					float(aNode.rotation[1]),
					float(aNode.rotation[2])));

			}
			if (aNode.scale.size() == 3)
			{
				nNode->AddScale(vec3(
					float(aNode.scale[0]),
					float(aNode.scale[1]),
					float(aNode.scale[2])));
			}
		}

		nNode->UpdateMatrix();

		// nNode->SetTransform(M, true);
		const vec3 mp = nNode->GetModelPosition();
		Info("\\%s %s Pos:[%.2f, %.2f, %.2f]", cbuf, name.c_str(),
			mp.x,
			mp.y,
			mp.z);

		if (!aNode.children.empty())
		{
			for (auto i : aNode.children)
			{
				Node3d* cNode = ImportNode(mModel.nodes[i], aLevel + 1);

				if (cNode) nNode->AddChildNode(cNode);
			}
		}


		if (aNode.mesh > -1)
		{
			const int mesh_idx = aNode.mesh;

			for (unsigned int j = meshOffsets[mesh_idx]; j < meshOffsets[mesh_idx + 1]; ++j) {
				nNode->AddRenderable(mScene.GetMeshByIndex(j));
			}
		}
		else if (light > -1)
		{
			/* not mesh */
			tinygltf::Light& tlight = mModel.lights[light];
			Info("\\%s light %s", cbuf, tlight.name.c_str());
			Vector3f lpos = vec3(0.f);
			tinygltf::Scene& scene = mModel.scenes[mModel.defaultScene];

			if (tlight.type == "point")
			{				
				const Vector3f ldiff = Color3(tlight.color[0], tlight.color[1], tlight.color[2]) * float(tlight.intensity);
				const Vector3f lspec = ldiff;;
				// Fatt = 1 / (1 + 2/r * d + 1/r2 * d2)

				auto p = std::make_shared<PointLight>(
					tlight.name,
					vec3(0.0f),
					ldiff, lspec,
					2.0f / mScene.mDefaultLightRadius, 1.0f / mScene.mDefaultLightRadius2, 0.0001f);

				nNode->AddRenderable(p);
				//nNode->SetPosition(lpos);

			}
		}
		return nNode;
	}
}
