#include "system/SystemTypes.hpp"
#include "scene/Node3d.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <algorithm>

namespace jse {

	Node3d::Node3d() : Node3d("unknown")
	{
	}

	Node3d::Node3d(const String& aName)
	{
		mName = aName;

		m_mtxLocal = Matrix(1.0f);
		m_mtxWorld = Matrix(1.0f);
		m_mtxModel = Matrix(1.0f);
		mPosition = Vector3f(0.f, 0.f, 0.f);
		mRotation = Quaternion(1.f, 0.f, 0.f, 0.f);
		mScale = Vector3f(1.f, 1.f, 1.f);
		mTransformUpdated = false;
		mVisible = false;
		mParentNode = NULL;
	}

	Node3d::Node3d(const String& aName, const unsigned int aMesh) : Node3d(aName)
	{
		mMeshes.push_back(aMesh);
	}

	Node3d::Node3d(const String& aName, Node3d* aParent) : Node3d(aName)
	{
		mParentNode = aParent;
	}

	Node3d::Node3d(const String& aName, Node3d* aParent, const unsigned int aMesh) : Node3d(aName, aParent)
	{
		mMeshes.push_back(aMesh);
	}

	void Node3d::SetPosition(const Vector3f& aPos)
	{
		mPosition = aPos;
		SetTransformUpdated();
	}

	void Node3d::SetRotation(const Quaternion& aRot)
	{
		mRotation = aRot;
		SetTransformUpdated();
	}

	void Node3d::SetScale(const Vector3f& aScale)
	{
		mScale = aScale;
		SetTransformUpdated();
	}

	void Node3d::SetScale(const float aUniformScale)
	{
		mScale = vec3(aUniformScale);
		SetTransformUpdated();
	}

	void Node3d::SetScale(const float aX, const float aY, const float aZ)
	{
		mScale.x = aX;
		mScale.y = aY;
		mScale.z = aZ;

		SetTransformUpdated();
	}

	void Node3d::SetTransform(const Matrix& aTransform, const bool aUpdate)
	{
		m_mtxLocal = aTransform;

		if (aUpdate)
			SetTransformUpdated();
	}

	void Node3d::SetWorldTransform(const Matrix& aTransform)
	{
		m_mtxWorld = aTransform;
	}

	void Node3d::SetVisible(const bool a0)
	{
		mVisible = a0;
	}
	
	const Matrix& Node3d::GetTransform() const
	{
		return m_mtxLocal;
	}

	void Node3d::AddChildNode(Node3d* aOther)
	{
		aOther->mParentNode = this;
		mChildNodes.push_back(aOther);
	}

	void Node3d::AddMesh(const unsigned int aMesh)
	{
		mMeshes.push_back(aMesh);
	}

	void Node3d::UpdateWorldTransform(const bool aUpdateChildren)
	{
		UpdateMatrix(aUpdateChildren);
	}

	void Node3d::UpdateMatrix(const bool aUpdateChildren)
	{
		if (!mTransformUpdated)
			return;

		mTransformUpdated = false;

		m_mtxModel = Matrix(1.0f);
		m_mtxModel = glm::translate(m_mtxModel, mPosition);
		m_mtxModel = glm::scale(m_mtxModel, mScale);
		m_mtxModel = m_mtxModel * glm::mat4_cast(mRotation);

		m_mtxWorld = mParentNode
			? mParentNode->m_mtxWorld * m_mtxLocal * m_mtxModel
			: m_mtxLocal * m_mtxModel;

		if (aUpdateChildren)
		{
			std::for_each(mChildNodes.begin(), mChildNodes.end(), [&](Node3d* const it) {
				it->UpdateMatrix(aUpdateChildren); 
			});
		}
	}

	void Node3d::SetTransformUpdated()
	{
		mTransformUpdated = true;
		if (!mChildNodes.empty())
		{
			std::for_each(mChildNodes.begin(), mChildNodes.end(), [&](Node3d* const it) {
				it->SetTransformUpdated();
			});
		}
	}

}

