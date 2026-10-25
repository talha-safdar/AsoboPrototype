#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera();

	glm::mat4 GetViewMatrix() const;

private:
	glm::vec3 m_position;
	glm::vec3 m_forward;
	glm::vec3 m_up;
};