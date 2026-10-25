#include "NavigationGrid.h"
#include <cmath>

NavigationGrid::NavigationGrid(int width, int height, float cellSize)
	: m_width(width),
	m_height(height),
	m_cellSize(cellSize),
	m_origin(-(width * cellSize) * 0.5f, 0.0f, -(height * cellSize) * 0.5f),
	m_walkable(width* height, true)
{}

/*
	Enemy world position:
	(3.4, 0, -2.1)

	↓ WorldToCell()

	Navigation cell:
	(x, z)
*/
glm::ivec2 NavigationGrid::WorldToCell(const glm::vec3& worldPosition) const
{
	int x = static_cast<int>(std::floor((worldPosition.x - m_origin.x) / m_cellSize));
	int z = static_cast<int>(std::floor((worldPosition.z - m_origin.z) / m_cellSize));

	return glm::ivec2(x, z);
}

/*
	Grid cell
	↓
	centre of that square
	↓
	world position enemy can walk toward
*/
glm::vec3 NavigationGrid::CellToWorld(const glm::ivec2& cell) const
{
	float x = m_origin.x + (cell.x + 0.5f) * m_cellSize;
	float z = m_origin.z + (cell.y + 0.5f) * m_cellSize;

	return glm::vec3(x, 0.0f, z);
}

// Does this cell actually exist inside our grid?
bool NavigationGrid::IsInside(const glm::ivec2& cell) const
{
	return cell.x >= 0 && cell.x < m_width && cell.y >= 0 && cell.y < m_height;
}

int NavigationGrid::GetIndex(const glm::ivec2& cell) const
{
	return cell.y * m_width + cell.x;
}

bool NavigationGrid::IsWalkable(const glm::ivec2& cell) const
{
	if (!IsInside(cell))
		return false;

	return m_walkable[GetIndex(cell)];
}

void NavigationGrid::Build(const std::vector<AABB>& obstacles)
{
	std::fill(m_walkable.begin(), m_walkable.end(), true);

	for (int z = 0; z < m_height; ++z)
	{
		for (int x = 0; x < m_width; ++x)
		{
			glm::ivec2 cell(x, z);
			glm::vec3 worldPosition = CellToWorld(cell);

			for (const AABB& obstacle : obstacles)
			{
				bool insideX = worldPosition.x >= obstacle.position.x - obstacle.halfSize.x && worldPosition.x <= obstacle.position.x + obstacle.halfSize.x;
				bool insideZ = worldPosition.z >= obstacle.position.z - obstacle.halfSize.z && worldPosition.z <= obstacle.position.z + obstacle.halfSize.z;

				if (insideX && insideZ)
				{
					m_walkable[GetIndex(cell)] = false;
					break;
				}
			}
		}
	}
}