#include "Pathfinder.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>

struct PathNode // only used while searching
{
	glm::ivec2 cell;
	float cost; // the A* score used to decide which cell to investigate next.

	bool operator>(const PathNode& other) const
	{
		return cost > other.cost;
	}
};

std::vector<glm::vec3> Pathfinder::FindPath(const NavigationGrid& grid, const glm::vec3& startWorld, const glm::vec3& endWorld) const
{
	glm::ivec2 start = grid.WorldToCell(startWorld);
	glm::ivec2 goal = grid.WorldToCell(endWorld);

	if (!grid.IsWalkable(start) || !grid.IsWalkable(goal))
		return {};

	int width = grid.GetWidth();
	int height = grid.GetHeight();
	int cellCount = width * height;

	auto GetIndex = [width](const glm::ivec2& cell)
		{
			return cell.y * width + cell.x;
		};

	std::vector<float> gCost(cellCount, std::numeric_limits<float>::max()); // How expensive was it to travel from the start to this cell?
	std::vector<glm::ivec2> parent(cellCount, glm::ivec2(-1, -1));
	std::vector<bool> closed(cellCount, false);

	std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> open;

	gCost[GetIndex(start)] = 0.0f;
	open.push({ start, 0.0f });

	const glm::ivec2 directions[] =
	{
		{ 1, 0 },
		{ -1, 0 },
		{ 0, 1 },
		{ 0, -1 }
	};

	while (!open.empty())
	{
		glm::ivec2 current = open.top().cell;
		open.pop();

		int currentIndex = GetIndex(current);

		if (closed[currentIndex])
			continue;

		closed[currentIndex] = true;

		if (current == goal)
			break;

		for (const glm::ivec2& direction : directions)
		{
			glm::ivec2 neighbour = current + direction;

			if (!grid.IsWalkable(neighbour))
				continue;

			int neighbourIndex = GetIndex(neighbour);

			if (closed[neighbourIndex])
				continue;

			float newGCost = gCost[currentIndex] + 1.0f;

			if (newGCost >= gCost[neighbourIndex])
				continue;

			gCost[neighbourIndex] = newGCost;
			parent[neighbourIndex] = current;

			// Roughly how far is this cell from the destination?
			// A* chooses cells with a low F.
			float heuristic = static_cast<float>(std::abs(goal.x - neighbour.x) + std::abs(goal.y - neighbour.y));
			float totalCost = newGCost + heuristic;

			open.push({ neighbour, totalCost });
		}
	}

	if (start != goal && parent[GetIndex(goal)] == glm::ivec2(-1, -1))
		return {};

	std::vector<glm::vec3> path;
	glm::ivec2 current = goal;

	while (current != start)
	{
		path.push_back(grid.CellToWorld(current));
		current = parent[GetIndex(current)];
	}

	std::reverse(path.begin(), path.end());

	return path;
}
