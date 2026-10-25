#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Mesh.h"

class Model
{
public:
	bool Load(const std::string& path);
	void Draw() const;

private:
	std::vector<std::unique_ptr<Mesh>> m_meshes;
};