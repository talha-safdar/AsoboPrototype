#include "Player.h"

#include <algorithm>
#include <cmath>
#include <glm/gtc/constants.hpp>

Player::Player() : 
	m_position(0.0f),
	m_direction(0.0f, 0.0f, -1.0f),
	m_yaw(glm::pi<float>()),
	m_verticalVelocity(0.0f),
	m_isGrounded(true)
{}

void Player::Update(const glm::vec3 & movementInput, bool jumpPressed, float deltaTime, const std::vector<AABB>&obstacles)
{
	const float movementSpeed = 3.0f;
	const float gravity = -9.81f;
	const float jumpVelocity = 5.5f;

	glm::vec3 movement = movementInput;

	if (glm::length(movement) > 0.0f)
	{
		movement = glm::normalize(movement);
		m_direction = movement;

		float targetYaw = atan2(m_direction.x, m_direction.z);
		float angleDifference = std::remainder(targetYaw - m_yaw, glm::two_pi<float>());
		float rotationAmount = std::clamp(10.0f * deltaTime, 0.0f, 1.0f);

		m_yaw += angleDifference * rotationAmount;

		float cosYaw = std::abs(std::cos(m_yaw));
		float sinYaw = std::abs(std::sin(m_yaw));

		float halfX = (0.3f * cosYaw) + (0.65f * sinYaw);
		float halfZ = (0.3f * sinYaw) + (0.65f * cosYaw);

		glm::vec3 playerHalfSize(halfX, 0.5f, halfZ);
		float movementAmount = movementSpeed * deltaTime;

		float previousX = m_position.x;
		m_position.x += movement.x * movementAmount;

		for (const AABB& obstacle : obstacles)
		{
			AABB playerBox{ m_position, playerHalfSize };

			if (Collision::IntersectsXZ(playerBox, obstacle))
			{
				m_position.x = previousX;
				break;
			}
		}

		float previousZ = m_position.z;
		m_position.z += movement.z * movementAmount;

		for (const AABB& obstacle : obstacles)
		{
			AABB playerBox{ m_position, playerHalfSize };

			if (Collision::IntersectsXZ(playerBox, obstacle))
			{
				m_position.z = previousZ;
				break;
			}
		}
	}

	if (jumpPressed && m_isGrounded)
	{
		m_verticalVelocity = jumpVelocity;
		m_isGrounded = false;
	}

	m_verticalVelocity += gravity * deltaTime;
	m_position.y += m_verticalVelocity * deltaTime;

	if (m_position.y <= 0.0f)
	{
		m_position.y = 0.0f;
		m_verticalVelocity = 0.0f;
		m_isGrounded = true;
	}
}

const glm::vec3& Player::GetPosition() const
{
	return m_position;
}

float Player::GetYaw() const
{
	return m_yaw;
}
