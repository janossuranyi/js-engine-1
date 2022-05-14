#include "scene/Light.hpp"
#include "graphics/GraphicsTypes.hpp"

namespace jse {

	PointLight::PointLight(const String& aName, const Vector3f& aPosition, const Vector3f& aDiffuse, const Vector3f& aSpecular, const float aConstantAtt, const float aLinearAtt, const float aQuadraticAtt)
		: Light(aName, LightType_Point)
	{
		mPosition = aPosition;
		mDiffuse = aDiffuse;
		mSpecular = aSpecular;
		mConstantAtt = aConstantAtt;
		mLinearAtt = aLinearAtt;
		mQuadraticAtt = aQuadraticAtt;
	}

	void PointLight::SetPosition(const Vector3f& aPosition)
	{
		mPosition = aPosition;
	}

	void PointLight::SetDiffuse(const Vector3f& aDiffuse)
	{
		mDiffuse = aDiffuse;
	}

	void PointLight::SetSpecular(const Vector3f& aSpecular)
	{
		mSpecular = aSpecular;
	}

	void PointLight::SetAttenuation(const float aConstantAtt, const float aLinearAtt, const float aQuadraticAtt)
	{
		mConstantAtt = aConstantAtt;
		mLinearAtt = aLinearAtt;
		mQuadraticAtt = aQuadraticAtt;
	}

	const Vector3f& PointLight::GetPosition() const
	{
		return mPosition;
	}
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
		return mConstantAtt;
	}
	float PointLight::GetLinearAtt() const
	{
		return mLinearAtt;
	}
	float PointLight::GetQuadraticAtt() const
	{
		return mQuadraticAtt;
	}
}