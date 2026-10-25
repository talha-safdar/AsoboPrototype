#pragma once

#include <glm/glm.hpp>

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
		Attack
	};

public:
	Enemy(const glm::vec3& position);

	void Update(const glm::vec3& playerPosition, float deltaTime);

	const glm::vec3& GetPosition() const;
	State GetState() const;

private:
	glm::vec3 m_position;

	State m_state;

	float m_moveSpeed;
	float m_detectionRange;
	float m_attackRange;
};