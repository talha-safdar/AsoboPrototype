#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera();

	glm::mat4 GetViewMatrix() const;

	void MoveForward(float amount);
	void MoveRight(float amount);

	void ProcessMouseMovement(float xOffset, float yOffset);

private:
	void UpdateVectors();

private:
	glm::vec3 m_position;
	glm::vec3 m_forward; // for W/S
	glm::vec3 m_up;
	glm::vec3 m_right; // for A/D

	float m_yaw;
	float m_pitch;
};