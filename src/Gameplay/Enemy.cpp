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

bool Enemy::Update(const glm::vec3& playerPosition, float deltaTime, const std::vector<AABB>& obstacles, const NavigationGrid& grid, const Pathfinder& pathfinder)
{
	if (m_attackTimer > 0.0f)
	{
		m_attackTimer -= deltaTime;
	}

	//if (m_pathRefreshTimer > 0.0f)
	//{
	//	m_pathRefreshTimer -= deltaTime;
	//}

	glm::vec3 toPlayer = playerPosition - m_position;
	toPlayer.y = 0.0f;

	float distanceToPlayer = glm::length(toPlayer);

	if (distanceToPlayer <= 0.001f)
	{
		return false;
	}

	glm::vec3 directionToPlayer = glm::normalize(toPlayer);

	float halfFov = glm::radians(m_fieldOfView * 0.5f);
	float fovThreshold = std::cos(halfFov);
	float facingAmount = glm::dot(m_forward, directionToPlayer);

	bool insideDetectionRange = distanceToPlayer <= m_detectionRange;
	bool insideFov = facingAmount >= fovThreshold;
	bool lineOfSightBlocked = false;

	if (insideDetectionRange && insideFov)
	{
		glm::vec3 enemyEye = m_position + glm::vec3(0.0f, 0.5f, 0.0f);
		glm::vec3 playerTarget = playerPosition + glm::vec3(0.0f, 0.5f, 0.0f);

		for (const AABB& obstacle : obstacles)
		{
			float hitDistance = 0.0f;

			if (Collision::LineIntersectsAABB(enemyEye, playerTarget, obstacle, hitDistance))
			{
				lineOfSightBlocked = true;
				break;
			}
		}
	}

	bool playerVisible = insideDetectionRange && insideFov && !lineOfSightBlocked;

	if (playerVisible)
	{
		m_lastKnownPlayerPosition = playerPosition;
		m_hasLastKnownPosition = true;

		float targetYaw = atan2(directionToPlayer.x, directionToPlayer.z);
		float angleDifference = std::remainder(targetYaw - m_yaw, glm::two_pi<float>());
		float rotationAmount = std::clamp(8.0f * deltaTime, 0.0f, 1.0f);

		m_yaw += angleDifference * rotationAmount;
		m_forward.x = std::sin(m_yaw);
		m_forward.y = 0.0f;
		m_forward.z = std::cos(m_yaw);

		// is player visible
		// if so cancel search path and normal chase resumes
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
		m_path.clear();
		m_currentWaypoint = 0;

		m_position += directionToPlayer * m_moveSpeed * deltaTime;

		return false;
	}

	if (!m_hasLastKnownPosition)
	{
		m_state = State::Idle;
		return false;
	}

	if (m_state != State::Search)
	{
		m_state = State::Search;
		m_path = pathfinder.FindPath(grid, m_position, m_lastKnownPlayerPosition);
		m_currentWaypoint = 0;
	}

	if (m_path.empty())
	{
		m_hasLastKnownPosition = false;
		m_state = State::Idle;
		return false;
	}

	if (m_currentWaypoint >= m_path.size())
	{
		m_path.clear();
		m_currentWaypoint = 0;
		m_hasLastKnownPosition = false;
		m_state = State::Idle;
		return false;
	}

	glm::vec3 waypoint = m_path[m_currentWaypoint];
	glm::vec3 toWaypoint = waypoint - m_position;
	toWaypoint.y = 0.0f;

	float waypointDistance = glm::length(toWaypoint);

	if (waypointDistance < 0.2f)
	{
		m_currentWaypoint++;

		if (m_currentWaypoint >= m_path.size())
		{
			m_path.clear();
			m_currentWaypoint = 0;
			m_hasLastKnownPosition = false;
			m_state = State::Idle;
		}

		return false;
	}

	glm::vec3 waypointDirection = glm::normalize(toWaypoint);

	float targetYaw = atan2(waypointDirection.x, waypointDirection.z);
	float angleDifference = std::remainder(targetYaw - m_yaw, glm::two_pi<float>());
	float rotationAmount = std::clamp(8.0f * deltaTime, 0.0f, 1.0f);

	m_yaw += angleDifference * rotationAmount;
	m_forward.x = std::sin(m_yaw);
	m_forward.y = 0.0f;
	m_forward.z = std::cos(m_yaw);

	m_position += waypointDirection * m_moveSpeed * deltaTime;

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
