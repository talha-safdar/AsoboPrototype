#pragma once

#include "Vertex.h"

#include <glad/gl.h>
#include <vector>

/*
* It owns
* VAO
* VBO
* EBO
*/
class Mesh
{
public:
	Mesh();
	~Mesh();

	/*
	* vertices → VBO // what the vertices are
	* indices  → EBO // which vertices should be connected
	* layout   → VAO // configuration describing how vertex data should be interpreted.
	*/
	bool Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

	/*
	* Bind VAO
	* glDrawElements()
	*/
	void Draw() const;

private:
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ebo;

	GLsizei m_indexCount;
};