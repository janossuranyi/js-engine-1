#ifndef JSE_ASSIMP_LOADER_H
#define JSE_ASSIMP_LOADER_H

#include "system/SystemTypes.hpp"
#include "system/Logger.hpp"
#include "graphics/GraphicsTypes.hpp"
#include "scene/SceneLoader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/pbrmaterial.h>

namespace jse {

	class Node3d;

	class AssimpLoader :public SceneLoader
	{
	public:
		AssimpLoader(Scene& aScene): mScene(aScene) {}
		~AssimpLoader() { Info("AssimpLoader destroyed"); }
		int LoadScene(const String& aFilename);
	private:
		void processNodesRecursive(const aiNode* node, Node3d* aParent, Matrix accTransform);

		Scene& mScene;
	};
}
#endif // !JSE_ASSIMP_LOADER_H
