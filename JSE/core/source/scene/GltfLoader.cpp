#include "scene/GltfLoader.hpp"
#include "scene/Mesh3d.hpp"
#include <tiny_gltf.h>

#include <algorithm>
#include <cassert>
#include <glm/gtc/type_ptr.hpp>

namespace jse {
	
    template<class T>
    const T* GltfLoader_cast_array(uint8_t const* byte_ptr)
    {
        return reinterpret_cast<T const*>(byte_ptr);
    }

    template<class T>
    inline float GltfLoader_cast_float(T a0)
    {
        return 1.0f * a0;
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
				tinygltf::Primitive p = m.primitives[j];
				tinygltf::Accessor indexAccessor = mModel.accessors[p.indices];

                tinygltf::BufferView& ibv = mModel.bufferViews[indexAccessor.bufferView];
                tinygltf::Buffer& ibuf = mModel.buffers[ibv.buffer];

                if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                {
                    uint8_t const* data = ibuf.data.data() + indexAccessor.byteOffset + ibv.byteOffset;
                    uint16_t const* iIn = GltfLoader_cast_array<uint16_t>(data);
                    for(int i1 = 0; i1 < indexAccessor.count; i1++)
                    {
                        dst->AddIndex(iIn[i1]);
                    }
                }
                
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
                    
                    const size_t byteStride = access.ByteStride(bv) / sizeof(float);

                    const uint8_t* buffer = buf.data.data() + access.byteOffset + bv.byteOffset;

                    float const * vIn = GltfLoader_cast_array<float>(buffer);
                    
                    size_t offset = 0;
					if (attrib.first.compare("POSITION") == 0)
					{
						for (size_t l = 0; l < access.count; l++)
						{
                            vec3 _v = glm::make_vec3( &vIn[ offset ] );
							verts.push_back(_v);
                            offset += byteStride;
                        }
					}
					else if (attrib.first.compare("NORMAL") == 0)
					{
						for (size_t l = 0; l < access.count; l++)
						{
                            vec3 _v = glm::make_vec3( &vIn[ offset ] );
							norms.push_back(_v);
                            offset += byteStride;
						}
					}
					else if (attrib.first.compare("TEXCOORD_0") == 0)
					{
						for (size_t l = 0; l < access.count; l++)
						{
                            vec2 _v = glm::make_vec2( &vIn[ offset ] );
							texco.push_back(_v);
                            offset += byteStride;
						}
					}
					else if (attrib.first.compare("TANGENT") == 0)
					{
						for (size_t l = 0; l < access.count; l++)
						{
							vec4 _v = glm::make_vec4( &vIn[ offset ]);
							tangs.push_back(_v);
                            offset += byteStride;
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
