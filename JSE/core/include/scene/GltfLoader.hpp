#ifndef JSE_GLTF_LOADER_H
#define JSE_GLTF_LOADER_H

#include "system/SystemTypes.hpp"
#include "system/Logger.hpp"
#include "graphics/GraphicsTypes.hpp"
#include "scene/SceneLoader.hpp"
#include "scene/Node3d.hpp"
#include <tiny_gltf.h>

namespace jse {

	class GltfLoader :public SceneLoader
	{
	public:
		GltfLoader(Scene& aScene) : mScene(aScene) {}
		~GltfLoader() { Info("GltfLoader destroyed"); }
		int LoadScene(const String& aFilename);
	private:

		Node3d* ImportNode(const tinygltf::Node& aNode);
		template<class T>
		void ExtractData(const tinygltf::Accessor& aAccessor, std::unique_ptr<T>& aDest);

		Scene& mScene;
		tinygltf::Model mModel;
		tinygltf::TinyGLTF mLoader;
		std::vector<unsigned int> meshOffsets;
	};
}
#endif // !JSE_ASSIMP_LOADER_H
