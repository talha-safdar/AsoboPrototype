#include "Enemy.h"

#include <glm/glm.hpp>

Enemy::Enemy(const glm::vec3& position) : 
	m_position(position),
	m_state(State::Idle),
	m_moveSpeed(2.0f),
	m_detectionRange(6.0f),
	m_attackRange(1.2f)
{

}

void Enemy::Update(const glm::vec3 & playerPosition, float deltaTime)
{
	glm::vec3 toPlayer = playerPosition - m_position;
	toPlayer.y = 0.0f;

	float distanceToPlayer = glm::length(toPlayer);

	if (distanceToPlayer <= m_attackRange)
	{
		m_state = State::Attack;
	}
	else if (distanceToPlayer <= m_detectionRange)
	{
		m_state = State::Chase;
	}
	else
	{
		m_state = State::Idle;
	}

	if (m_state == State::Chase && distanceToPlayer > 0.001f)
	{
		glm::vec3 direction = glm::normalize(toPlayer);
		m_position += direction * m_moveSpeed * deltaTime;
	}
}

const glm::vec3& Enemy::GetPosition() const
{
	return m_position;
}

Enemy::State Enemy::GetState() const
{
	return m_state;
}
