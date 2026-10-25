#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../Physics/Collision.h"

/*
	. . . . . .
	. . X X . .
	. . X X . .
	. . . . . .
	. . . . . .

	. = enemy can walk there
	X = obstacle
	
	e.g. NavigationGrid(20, 20, 1.0f);
	20 × 20 cells
	each cell = 1 world unit
*/
class NavigationGrid
{
public:
	NavigationGrid(int width, int height, float cellSize);

	void Build(const std::vector<AABB>& obstacles);

	glm::ivec2 WorldToCell(const glm::vec3& worldPosition) const;
	glm::vec3 CellToWorld(const glm::ivec2& cell) const;

	bool IsWalkable(const glm::ivec2& cell) const;
	bool IsInside(const glm::ivec2& cell) const;

private:
	int GetIndex(const glm::ivec2& cell) const;

private:
	int m_width;
	int m_height;
	float m_cellSize;
	glm::vec3 m_origin;

	std::vector<bool> m_walkable;
};