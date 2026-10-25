#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../Physics/Collision.h"

/*
	It owns:
	position
	direction
	rotation
	vertical velocity
	grounded state
*/
class Player
{
public:
	Player();

	void Update(const glm::vec3& movementInput, bool jumpPressed, float deltaTime, const std::vector<AABB>& obstacles);

	const glm::vec3& GetPosition() const;
	float GetYaw() const;

private:
	glm::vec3 m_position;
	glm::vec3 m_direction;

	float m_yaw;
	float m_verticalVelocity;

	bool m_isGrounded;
};