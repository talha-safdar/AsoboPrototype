#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera();

	void Update(const glm::vec3& target);
	void ProcessMouseMovement(float xOffset, float yOffset);
	glm::mat4 GetViewMatrix(const glm::vec3& target) const;
	glm::vec3 GetForward() const;
	glm::vec3 GetRight() const;
	const glm::vec3& GetPosition() const; // lets Application inspect where the camera wants to be.
	void SetPosition(const glm::vec3& position); // lets us correct it if that position would go through an obstacle.

private:
	void UpdateVectors();

private:
	glm::vec3 m_position;
	glm::vec3 m_forward; // for W/S
	glm::vec3 m_up;
	glm::vec3 m_right; // for A/D

	float m_yaw;
	float m_pitch;
	float m_distance; // How far should the camera stay from the player?
};