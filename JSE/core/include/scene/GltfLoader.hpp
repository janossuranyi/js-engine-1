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

		void RecursiveProcessNode(const tinygltf::Node& aNode, Node3d* aParent, const Matrix& accTransform, int level = 0);

		Scene& mScene;
		tinygltf::Model mModel;
		tinygltf::TinyGLTF mLoader;

	};
}
#endif // !JSE_ASSIMP_LOADER_H
