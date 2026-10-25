#pragma once

#include <glad/gl.h>
#include <vector>

class Mesh
{
public:
	Mesh();
	~Mesh();

	bool Create(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

	void Draw() const;

private:
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ebo;

	GLsizei m_indexCount;
};