#pragma once

#include <glm/glm.hpp>

struct AABB
{
	// position = centre of box
	// halfSize = distance from centre to each side
	glm::vec3 position;
	glm::vec3 halfSize;
};

namespace Collision
{
	bool LineIntersectsAABB(const glm::vec3& start, const glm::vec3& end, const AABB& box, float& hitDistance);
}