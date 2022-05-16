#ifndef JSE_NODE3D_H
#define JSE_NODE3D_H

#include <vector>

#include "graphics/GraphicsTypes.hpp"
#include "system/SystemTypes.hpp"
#include "scene/Mesh3d.hpp"
#include "scene/Animation.hpp"

namespace jse {

	class Mesh3d;
	class Node3d;

	typedef std::vector<Node3d*> Node3dPtrVec;
	typedef std::vector<unsigned int> MeshIdxVec;

	class Node3d
	{
	public:
		Node3d();
		Node3d(const String& aName);
		Node3d(const String& aName, const unsigned int aMesh);
		Node3d(const String& aName, Node3d* aParent);
		Node3d(const String& aName, Node3d* aParent, const unsigned int aMesh);

		void SetPosition(const Vector3f& aPos);
		void AddPosition(const Vector3f& aPos);
		void SetRotation(const Quaternion& aRot);
		void AddRotation(const Quaternion& aRot);
		
		void SetScale(const Vector3f& aScale);
		void AddScale(const Vector3f& aScale);
		void SetScale(const float aUniformScale);
		void SetScale(const float aX, const float aY, const float aZ);
		void SetTransform(const Matrix& aTransform, const bool aUpdate);
		void SetWorldTransform(const Matrix& aTransform);
		void SetVisible(const bool a0);
		const Matrix& GetTransform() const;
		void AddChildNode(Node3d* aOther);
		void AddMesh(const unsigned int aMesh);
		inline bool GetTransformUpdated() const { return mTransformUpdated; }
		void UpdateWorldTransform(const bool aUpdateChildren);
		void UpdateMatrix(const bool aUpdateChildren);
		inline const Matrix& GetWorldTransform() const { return m_mtxWorld; };
		inline unsigned int GetMesh(const unsigned aIdx) { return mMeshes[aIdx]; }
		inline const Node3dPtrVec& GetChildren() const { return mChildNodes; }
		inline bool HasChildren() const { return !mChildNodes.empty(); }
		inline const MeshIdxVec& GetMeshes() const { return mMeshes; }
		const String& GetName() const { return mName; }
		inline const Vector3f& GetPostion() const { return mPosition; }
		inline const Vector3f& GetScale() const { return mScale; }
		inline const Quaternion& GetRotation() const { return mRotation; }
		inline const bool IsVisible() const { return mVisible; }

		Animation* CreateAnimation(const String& aName);
		bool HasAnimation() const { return mAnimation != nullptr; }
		Animation* GetAnimation() const { return mAnimation; }

	private:
		void SetTransformUpdated();

		String mName;

		Matrix m_mtxLocal;
		Matrix m_mtxWorld;
		Matrix m_mtxModel;

		Vector3f mPosition;
		Quat mRotation;
		Vector3f mScale;

		Node3d* mParentNode;
		Node3dPtrVec mChildNodes;

		Animation* mAnimation;
		MeshIdxVec mMeshes;
		bool mTransformUpdated;
		bool mVisible;

	};

}
#endif