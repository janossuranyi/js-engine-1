#include "scene/Light.hpp"
#include "graphics/GraphicsTypes.hpp"

namespace jse {

	PointLight::PointLight(const String& aName, const Vector3f& aPosition, const Vector3f& aDiffuse, const Vector3f& aSpecular, const float aLinearAtt, const float aQuadraticAtt, const float aCutoff)
		: Light(aName, LightType_Point)
	{
		mPosition = aPosition;
		mDiffuse = aDiffuse;
		mSpecular = aSpecular;
		mLinearAtt = aLinearAtt;
		mQuadraticAtt = aQuadraticAtt;
		mCutoff = aCutoff;
	}

	//void PointLight::SetPosition(const Vector3f& aPosition)
	//{
	//	mPosition = aPosition;
	//}

	void PointLight::SetDiffuse(const Vector3f& aDiffuse)
	{
		mDiffuse = aDiffuse;
	}

	void PointLight::SetSpecular(const Vector3f& aSpecular)
	{
		mSpecular = aSpecular;
	}

	void PointLight::SetCutOff(const float aCutoff)
	{
		mCutoff = aCutoff;
	}

	void PointLight::SetAttenuation(const float aLinearAtt, const float aQuadraticAtt)
	{
		mLinearAtt = aLinearAtt;
		mQuadraticAtt = aQuadraticAtt;
	}

	//const Vector3f& PointLight::GetPosition() const
	//{
	//	return mPosition;
	//}
	const Vector3f& PointLight::GetDiffuse() const
	{
		return mDiffuse;
	}
	const Vector3f& PointLight::GetSpecular() const
	{
		return mSpecular;
	}
	float PointLight::GetConstantAtt() const
	{
		return 1.0f;
	}
	float PointLight::GetLinearAtt() const
	{
		return mLinearAtt;
	}
	float PointLight::GetQuadraticAtt() const
	{
		return mQuadraticAtt;
	}
	float PointLight::GetCutOff() const
	{
		return mCutoff;
	}
}