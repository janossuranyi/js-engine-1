#include "scene/Camera.hpp"

namespace jse {
	void Camera::SetPosition(const vec3& aVec)
	{
		position = aVec;
	}
	void Camera::MoveForward(const float aTime)
	{
		position += speed * aTime * direction;
	}
	void Camera::MoveBackward(const float aTime)
	{
		position += -speed * aTime * direction;
	}
	void Camera::MoveLeft(const float aTime)
	{
		position += glm::normalize(glm::cross(direction, up)) * speed * aTime;
	}
	void Camera::MoveRight(const float aTime)
	{
		position -= glm::normalize(glm::cross(direction, up)) * speed * aTime;
	}
	void Camera::TurnLeft(const float aTime)
	{
		yaw += -speed * aTime;
	}
	void Camera::TurnRight(const float aTime)
	{
		yaw += speed * aTime;
	}
	void Camera::TurnUp(const float aTime)
	{
		pitch += speed * aTime;
	}
	void Camera::TurnDown(const float aTime)
	{
		pitch += speed * aTime;
	}
	void Camera::AddPitch(const float aX)
	{
		pitch += aX;
	}
	void Camera::AddYaw(const float aX)
	{
		yaw += aX;
	}
	void Camera::SetPitch(const float aX)
	{
		pitch = aX;
	}
	void Camera::SetYaw(const float aX)
	{
		yaw = aX;
	}
	void Camera::SetDirection(const float aYaw, const float aPitch)
	{
		yaw = aYaw;
		pitch = aPitch;
	}
	mat4 Camera::GetViewMatrix()
	{
		// calculate directon vector
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		return glm::lookAt(position, position + direction, up);
	}
}
