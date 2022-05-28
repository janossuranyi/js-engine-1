#ifndef JSE_GLTF_LOADER_H
#define JSE_GLTF_LOADER_H

#include "system/SystemTypes.hpp"
#include "system/Logger.hpp"
#include "graphics/GraphicsTypes.hpp"
#include "scene/SceneLoader.hpp"
#include <tiny_gltf.h>

namespace jse {

	class GltfLoader :public SceneLoader
	{
	public:
		GltfLoader(Scene& aScene) : mScene(aScene) {}
		~GltfLoader() { Info("GltfLoader destroyed"); }
		int LoadScene(const String& aFilename);
	private:

		Scene& mScene;
		tinygltf::Model mModel;
		tinygltf::TinyGLTF mLoader;

	};
}
#endif // !JSE_ASSIMP_LOADER_H
