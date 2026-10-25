#include "Enemy.h"

#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include <glm/gtc/constants.hpp>

Enemy::Enemy(const glm::vec3& position) : 
	m_position(position),
	m_forward(0.0f, 0.0f, 1.0f),
	m_state(State::Idle),
	m_moveSpeed(2.0f),
	m_detectionRange(6.0f),
	m_attackRange(1.2f),
	m_fieldOfView(90.0f),
	m_attackCooldown(1.0f),
	m_attackTimer(0.0f),
	m_yaw(0.0f)
{

}

bool Enemy::Update(const glm::vec3& playerPosition, float deltaTime, const std::vector<AABB>& obstacles)
{
	if (m_attackTimer > 0.0f)
	{
		m_attackTimer -= deltaTime;
	}

	glm::vec3 toPlayer = playerPosition - m_position;
	toPlayer.y = 0.0f;

	float distanceToPlayer = glm::length(toPlayer);

	if (distanceToPlayer > m_detectionRange || distanceToPlayer <= 0.001f) // Too far away → can't see you.
	{
		m_state = State::Idle;
		return false;
	}

	glm::vec3 directionToPlayer = glm::normalize(toPlayer);

	// calculate yaw the turining left right towards the player
	float targetYaw = atan2(directionToPlayer.x, directionToPlayer.z);
	float angleDifference = std::remainder(targetYaw - m_yaw, glm::two_pi<float>());
	float rotationAmount = std::clamp(8.0f * deltaTime, 0.0f, 1.0f);

	m_yaw += angleDifference * rotationAmount;
	m_forward.x = std::sin(m_yaw);
	m_forward.y = 0.0f;
	m_forward.z = std::cos(m_yaw);


	float halfFov = glm::radians(m_fieldOfView * 0.5f);
	float fovThreshold = std::cos(halfFov);
	float facingAmount = glm::dot(m_forward, directionToPlayer); // Is the player actually in front of the enemy?

	if (facingAmount < fovThreshold)
	{
		m_state = State::Idle;
		return false;
	}

	glm::vec3 enemyEye = m_position + glm::vec3(0.0f, 0.5f, 0.0f);
	glm::vec3 playerTarget = playerPosition + glm::vec3(0.0f, 0.5f, 0.0f);

	for (const AABB& obstacle : obstacles)
	{
		float hitDistance = 0.0f;

		if (Collision::LineIntersectsAABB(enemyEye, playerTarget, obstacle, hitDistance))
		{
			m_state = State::Idle;
			return false;
		}
	}

	if (distanceToPlayer <= m_attackRange)
	{
		m_state = State::Attack;

		if (m_attackTimer <= 0.0f)
		{
			m_attackTimer = m_attackCooldown;
			return true;
		}

		return false;
	}

	m_state = State::Chase;
	m_forward = directionToPlayer;
	m_position += directionToPlayer * m_moveSpeed * deltaTime;

	return false;
}

const glm::vec3& Enemy::GetPosition() const
{
	return m_position;
}

Enemy::State Enemy::GetState() const
{
	return m_state;
}

float Enemy::GetYaw() const
{
	return m_yaw;
}
