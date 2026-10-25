#include "Collision.h"

bool Collision::LineIntersectsAABB(const glm::vec3& start, const glm::vec3& end, const AABB& box, float& hitDistance)
{
	// Is a wall / cube blocking the enemy's view of the player?

	glm::vec3 direction = end - start;
	float length = glm::length(direction);

	if (length <= 0.0001f)
		return false;

	direction = glm::normalize(direction);

	glm::vec3 boxMin = box.position - box.halfSize;
	glm::vec3 boxMax = box.position + box.halfSize;

	float tMin = 0.0f;
	float tMax = length;

	for (int axis = 0; axis < 3; ++axis)
	{
		if (std::abs(direction[axis]) < 0.0001f)
		{
			if (start[axis] < boxMin[axis] || start[axis] > boxMax[axis])
				return false;

			continue;
		}

		float t1 = (boxMin[axis] - start[axis]) / direction[axis];
		float t2 = (boxMax[axis] - start[axis]) / direction[axis];

		if (t1 > t2)
			std::swap(t1, t2);

		tMin = std::max(tMin, t1);
		tMax = std::min(tMax, t2);

		if (tMin > tMax)
			return false;
	}

	hitDistance = tMin;
	return true;
}

bool Collision::IntersectsXZ(const AABB& a, const AABB& b)
{
	bool overlapX = std::abs(a.position.x - b.position.x) < (a.halfSize.x + b.halfSize.x);
	bool overlapZ = std::abs(a.position.z - b.position.z) < (a.halfSize.z + b.halfSize.z);

	return overlapX && overlapZ;
}
