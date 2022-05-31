#ifndef JSE_NODE3D_H
#define JSE_NODE3D_H

#include <vector>

#include "graphics/GraphicsTypes.hpp"
#include "system/SystemTypes.hpp"
#include "scene/Mesh3d.hpp"
#include "scene/Light.hpp"
#include "scene/Animation.hpp"

namespace jse {

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
		void SetVisible(const bool a0);
		void SetAnimated(const bool a0) { mAnimated = a0; }
		const Matrix& GetTransform() const;

		void AddChildNode(Node3d* aOther);
		void AddMesh(const unsigned int aMesh);
		void AddLight(const unsigned int aLight);
		inline bool GetTransformUpdated() const { return mTransformUpdated; }
		void UpdateWorldTransform(const bool aUpdateChildren);
		void UpdateMatrix(const bool aUpdateChildren);
		inline const Matrix& GetWorldTransform() const { return m_mtxWorld; };
		inline unsigned int GetMesh(const unsigned aIdx) { return mMeshes[aIdx]; }
		inline const Node3dPtrVec& GetChildren() const { return mChildNodes; }
		inline const std::vector<unsigned int>& GetLights() const { return mLights; }
		inline bool HasChildren() const { return !mChildNodes.empty(); }
		inline const MeshIdxVec& GetMeshes() const { return mMeshes; }
		const String& GetName() const { return mName; }
		inline const Vector3f& GetPostion() const { return mPosition; }
		inline const Vector3f& GetScale() const { return mScale; }
		inline const Quaternion& GetRotation() const { return mRotation; }
		inline const bool IsVisible() const { return mVisible; }
		inline const bool IsAnimated() const { return mAnimated; }

	private:
		void SetTransformUpdated();

		String mName;

		Matrix m_mtxWorld;
		Matrix m_mtxModel;

		Vector3f mPosition;
		Quat mRotation;
		Vector3f mScale;

		Node3d* mParentNode;
		Node3dPtrVec mChildNodes;

		MeshIdxVec mMeshes;
		
		std::vector<unsigned int> mLights;

		bool mTransformUpdated;
		bool mVisible;
		bool mAnimated;

	};

}
#endif