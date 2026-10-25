#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "NavigationGrid.h"

class Pathfinder
{
public:
	std::vector<glm::vec3> FindPath(const NavigationGrid& grid, const glm::vec3& startWorld, const glm::vec3& endWorld) const;
};