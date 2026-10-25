#pragma once

#include <glm/glm.hpp>

struct AABB
{
	// position = centre of box
	// halfSize = distance from centre to each side
	glm::vec3 position;
	glm::vec3 halfSize;
};

/*
* has no state and no object identity. It is just a group of reusable math functions.
* these act like static functions so dont need Collision collision;
*/
namespace Collision
{
	bool LineIntersectsAABB(const glm::vec3& start, const glm::vec3& end, const AABB& box, float& hitDistance);

	bool IntersectsXZ(const AABB& a, const AABB& b);
}