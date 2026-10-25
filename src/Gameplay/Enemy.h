#pragma once

#include "../Physics/Collision.h"

#include <glm/glm.hpp>
#include <vector>
#include "../Navigation/NavigationGrid.h"
#include "../Navigation/Pathfinder.h"

/*
	It owns:
	position
	current AI state
	movement speed
	detection distance
	attack distance
*/
class Enemy
{
public:
	enum class State
	{
		Idle,
		Chase,
		Search,
		Attack
	};

public:
	Enemy(const glm::vec3& position);

	// bool means Did the enemy successfully attack this frame?
	bool Update(const glm::vec3& playerPosition, float deltaTime, const std::vector<AABB>& obstacles, const NavigationGrid& grid, const Pathfinder& pathfinder);

	const glm::vec3& GetPosition() const;
	State GetState() const;
	float GetYaw() const;

private:
	glm::vec3 m_position;

	State m_state;

	float m_moveSpeed;
	float m_detectionRange;
	float m_attackRange;

	glm::vec3 m_forward;
	float m_fieldOfView;

	float m_attackCooldown; // attackCooldown = attack every 1 second maximum
	float m_attackTimer; // attackTimer = time remaining before next attack
	float m_yaw;

	std::vector<glm::vec3> m_path;
	std::size_t m_currentWaypoint = 0;
	glm::vec3 m_lastKnownPlayerPosition{ 0.0f };
	bool m_hasLastKnownPosition = false;
	// float m_pathRefreshTimer = 0.0f;
};