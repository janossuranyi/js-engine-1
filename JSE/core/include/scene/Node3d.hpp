#ifndef JSE_NODE3D_H
#define JSE_NODE3D_H

#include <vector>
#include <memory>

#include "graphics/GraphicsTypes.hpp"
#include "system/SystemTypes.hpp"
#include "scene/Mesh3d.hpp"
#include "scene/Light.hpp"
#include "scene/Animation.hpp"

namespace jse {

	class Renderable;

	typedef std::vector<Node3d*> Node3dPtrVec;
	typedef std::vector<std::shared_ptr<Renderable>> RenderablePtrVec;

	class Node3d
	{
	public:
		Node3d();
		Node3d(const String& aName);
		Node3d(const String& aName, Node3d* aParent);

		void SetPosition(const Vector3f& aPos);
		void AddPosition(const Vector3f& aPos);
		void AddRotation(const Quaternion& aRot);
		
		void AddScale(const Vector3f& aScale);
		void SetTransform(const Matrix& aTransform, const bool aUpdate);
		void UpdateRTS();
		void SetVisible(const bool a0);
		const Matrix& GetModelMatrix() const;

		void AddChildNode(Node3d* aOther);
		void SetParent(Node3d* aParent);
		void AddRenderable(std::shared_ptr<Renderable> a0);

		inline bool GetTransformUpdated() const { return mTransformUpdated; }
		void UpdateWorldTransform();
		void UpdateMatrix();
		inline const Matrix& GetWorldMatrix() const { return m_mtxWorld; };
		inline const Node3dPtrVec& GetChildren() const { return mChildNodes; }
		inline bool HasChildren() const { return !mChildNodes.empty(); }
		inline const RenderablePtrVec& GetRenderables() const { return mRenderableVec; }
		const String& GetName() const { return mName; }
		inline const Vector3f GetModelPosition() const { return m_mtxModel[3]; }
		inline const Vector3f GetWorldPosition() const { return m_mtxWorld[3]; }
		inline const bool IsVisible() const { return mVisible; }
		void SetTransformUpdated();

	private:
		

		String mName;

		Matrix m_mtxWorld;
		Matrix m_mtxModel;

		Vector3f mPosition;
		Quat mRotation;
		Vector3f mScale;

		Node3d* mParentNode;
		Node3dPtrVec mChildNodes;

		RenderablePtrVec mRenderableVec;

		bool mTransformUpdated;
		bool mVisible;

	};

}
#endif