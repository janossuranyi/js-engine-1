#ifndef JSE_LIGHT_H
#define JSE_LIGHT_H

#include "graphics/GraphicsTypes.hpp"
#include "graphics/Renderable.hpp"

namespace jse {

	enum LightType
	{
		LightType_Point,
		LightType_Spot,
		LightType_Directional,
		LightType_LastEnum
	};

	class Light : public Renderable
	{
	public:
		Light(const String& aName, const LightType aType) : mName(aName), mType(aType) {};
		~Light() {}
		void Draw() {}
		const String& GetName() const { return mName; }
		LightType GetLightType() const { return mType; }
		RenderableType GetType() const { return RenderableType::Light; }
	private:
		String mName;
		LightType mType;
	};

	class PointLight : public Light
	{
	public:
		PointLight(const String& aName,
			const Vector3f& aPosition = Vector3f(0.0f),
			const Vector3f& aDiffuse = Vector3f(0.0f),
			const Vector3f& aSpecular = Vector3f(0.0f),
			const float aLinearAtt = 1,
			const float aQuadraticAtt = 1,
			const float aCutoff = 0.0005);

		~PointLight() {}

		void SetPosition(const Vector3f& aPosition);
		void SetDiffuse(const Vector3f& aDiffuse);
		void SetSpecular(const Vector3f& aSpecular);
		void SetCutOff(const float aCutoff);
		void SetAttenuation(
			const float aLinearAtt,
			const float aQuadraticAtt);

		const Vector3f& GetPosition() const;
		const Vector3f& GetDiffuse() const;
		const Vector3f& GetSpecular() const;

		float GetConstantAtt() const;
		float GetLinearAtt() const;
		float GetQuadraticAtt() const;
		float GetCutOff() const;

	private:
		Vector3f mPosition;
		Vector3f mDiffuse;
		Vector3f mSpecular;

		float mLinearAtt;
		float mQuadraticAtt;
		float mCutoff;
	};
}
#endif