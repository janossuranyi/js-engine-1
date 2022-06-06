#ifndef JSE_CAMERA_H
#define JSE_CAMERA_H

#include "graphics/GraphicsTypes.hpp"
#include <glm/glm.hpp>

namespace jse {
	class Camera
	{
	public:
		Camera() {}
		~Camera() {}

		vec3 GetPosition() const { return position; }
		vec3 GetDirection() const { return direction; }
		vec3 GetUpVector() const { return up; }
		vec3 GetTarget() const;
		float GetSpeed() const { return speed; }
		void SetSpeed(const float aSpeed) { speed = aSpeed; }

		void SetPosition(const vec3& aVec);
		void MoveForward(const float aTime);
		void MoveBackward(const float aTime);
		void MoveLeft(const float aTime);
		void MoveRight(const float aTime);
		void TurnLeft(const float aTime);
		void TurnRight(const float aTime);
		void TurnUp(const float aTime);
		void TurnDown(const float aTime);
		void AddPitch(const float aX);
		void AddYaw(const float aX);
		void SetPitch(const float aX);
		void SetYaw(const float aX);
		void SetDirection(const float aYaw, const float aPitch);

		mat4 GetViewMatrix();

	private:
		vec3 position{ 0, 0, 0 };
		vec3 direction{ 0,0,-1 };
		vec3 up{ 0,1,0 };
		float speed{ 0.05f };
		float pitch{ 0 };
		float yaw{ -90 };

	};
}
#endif // !JSE_CAMERA_H
