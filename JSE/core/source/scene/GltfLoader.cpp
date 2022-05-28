#include "scene/GltfLoader.hpp"
#include "scene/Mesh3d.hpp"
#include <tiny_gltf.h>

#include <algorithm>
#include <cassert>
#include <glm/gtc/type_ptr.hpp>

namespace jse {
	

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

		std::vector<vec3> verts;
		std::vector<vec3> norms;
		std::vector<vec2> texco;
		std::vector<vec4> tangs;

		for (size_t i = 0; i < mModel.meshes.size(); i++)
		{
			const tinygltf::Mesh& m = mModel.meshes[i];
			Mesh3d* dst = new Mesh3d(m.name);

			verts.clear();
			norms.clear();
			texco.clear();
			tangs.clear();

			for (size_t j = 0; j < m.primitives.size(); j++)
			{
				VertexData vtx;
				tinygltf::Primitive p = m.primitives[j];
				tinygltf::Accessor indexAccessor = mModel.accessors[p.indices];


				for (auto& attrib : p.attributes)
				{
					tinygltf::Accessor access = mModel.accessors[attrib.second];
					tinygltf::BufferView& bv = mModel.bufferViews[access.bufferView];
					tinygltf::Buffer& buf = mModel.buffers[bv.buffer];

					size_t byteStride = access.ByteStride(bv);

					const uint8_t* buffer = buf.data.data() + access.byteOffset + bv.byteOffset;
					
					if (attrib.first.compare("POSITION") == 0 && access.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
					{
						for (size_t l = 0; l < access.count; l++)
						{
							vec3 _v = glm::make_vec3(reinterpret_cast<float const*>(buffer + l * byteStride));
							verts.push_back(_v);
						}
					}
					else if (attrib.first.compare("NORMAL") == 0 && access.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
					{
						for (size_t l = 0; l < access.count; l++)
						{
							vec3 _v = glm::make_vec3(reinterpret_cast<float const*>(buffer + l * byteStride));
							norms.push_back(_v);
						}
					}
					else if (attrib.first.compare("TEXCOORD_0") == 0 && access.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
					{
						for (size_t l = 0; l < access.count; l++)
						{
							vec2 _v = glm::make_vec2(reinterpret_cast<float const*>(buffer + l * byteStride));
							texco.push_back(_v);
						}
					}
					else if (attrib.first.compare("TANGENT") == 0 && access.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
					{
						for (size_t l = 0; l < access.count; l++)
						{
							vec4 _v = glm::make_vec4(reinterpret_cast<float const*>(buffer + l * byteStride));
							tangs.push_back(_v);
						}
					}
				}

				for (size_t l = 0; l < verts.size(); l++)
				{
					VertexData vtx;
					vtx.position = verts[l];
					vtx.normal = norms[l];
					vtx.texcoord = texco[l];
					vtx.tangent = tangs[l];
					vtx.bitangent = glm::cross(vtx.normal, vtx.tangent) * tangs[l].w;
					dst->AddVertex(vtx);
				}
			}

		}

		for (size_t i = 0; i < scene.nodes.size(); i++)
		{

		}


		return 0;
	}


}