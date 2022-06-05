#include "system/SystemTypes.hpp"
#include "scene/Node3d.hpp"
#include "scene/Animation.hpp"

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

		m_mtxWorld = Matrix(1.0f);
		m_mtxModel = Matrix(1.0f);
		mPosition = Vector3f(0.f, 0.f, 0.f);
		mRotation = Quaternion(1.f, 0.f, 0.f, 0.f);
		mScale = Vector3f(1.f, 1.f, 1.f);
		mTransformUpdated = false;
		mVisible = false;
		mParentNode = nullptr;
	}

	Node3d::Node3d(const String& aName, Node3d* aParent) : Node3d(aName)
	{
		mParentNode = aParent;
		SetTransformUpdated();
	}

	void Node3d::SetPosition(const Vector3f& aPos)
	{
		m_mtxModel[3].x = aPos.x;
		m_mtxModel[3].y = aPos.y;
		m_mtxModel[3].z = aPos.z;

		SetTransformUpdated();
	}

	void Node3d::AddPosition(const Vector3f& aPos)
	{
		vec3 vpos = GetModelPosition();
		mPosition += (aPos - vpos);
	}

	void Node3d::AddRotation(const Quaternion& aRot)
	{
		mRotation = aRot * mRotation;
	}

	void Node3d::AddScale(const Vector3f& aScale)
	{
		mScale *= aScale;
	}

	void Node3d::SetTransform(const Matrix& aTransform, const bool aUpdate)
	{
		m_mtxModel = aTransform;

		if (aUpdate)
			SetTransformUpdated();
	}

	void Node3d::UpdateRTS()
	{
		vec3 tmp1;
		vec4 tmp2;

		glm::decompose(m_mtxModel, mScale, mRotation, mPosition, tmp1, tmp2);
	}

	void Node3d::SetVisible(const bool a0)
	{
		mVisible = a0;
	}
	
	const Matrix& Node3d::GetModelMatrix() const
	{
		return m_mtxModel;
	}

	void Node3d::AddChildNode(Node3d* aOther)
	{
		aOther->SetParent(this);
		mChildNodes.push_back(aOther);
	}

	void Node3d::SetParent(Node3d* aParent)
	{
		mParentNode = aParent;
		SetTransformUpdated();
	}

	void Node3d::AddRenderable(std::shared_ptr<Renderable> a0)
	{
		mRenderableVec.push_back(a0);
	}

	void Node3d::UpdateWorldTransform()
	{
		if (mTransformUpdated)
		{
			mTransformUpdated = false;
			if (mParentNode)
			{
				m_mtxWorld = mParentNode->GetWorldMatrix() * m_mtxModel;
			}
			else
			{
				m_mtxWorld = m_mtxModel;
			}
		}
	}

	void Node3d::UpdateMatrix()
	{

		vec3 vPos = m_mtxModel[3];

		Matrix mtxModel = Matrix(1.f);

		mtxModel = glm::translate(mtxModel, vPos + mPosition);
		mtxModel = mtxModel * glm::mat4_cast(mRotation);

		mtxModel = glm::scale(mtxModel, mScale);


		SetTransform(mtxModel, true);

		mRotation = Quat(1, 0, 0, 0);
		mPosition = vec3(0.0f);
		mScale = vec3(1.0f);
	}

	void Node3d::SetTransformUpdated()
	{
		mTransformUpdated = true;

		if (!mChildNodes.empty())
		{
			for(auto it : mChildNodes)
				it->SetTransformUpdated();
		}
	}

}

