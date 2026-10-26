#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

#include "Mesh.h"

class Shader;

struct ModelPart
{
	/*
		name            → "RightArm"
		mesh            → actual geometry
		transform       → original transform from the GLB
		localTransform  → OUR extra modification
	*/
	std::string name;
	std::unique_ptr<Mesh> mesh;
	glm::mat4 transform{ 1.0f };
	glm::mat4 localTransform{ 1.0f };
};

class Model
{
public:
	bool Load(const std::string& path);
	void Draw(Shader& shader, const glm::mat4& modelMatrix) const;

	void RotatePart(const std::string& name, float angle, const glm::vec3& axis);
	void ResetPartTransform(const std::string& name);
	void PrintParts() const;

private:
	std::vector<ModelPart> m_parts; // this geometry belongs HERE inside the imported model.
};