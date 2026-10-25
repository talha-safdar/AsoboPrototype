#include "Mesh.h"

Mesh::Mesh() : m_vao(0), m_vbo(0), m_ebo(0), m_indexCount(0)
{}

Mesh::~Mesh()
{
    if (m_ebo)
    {
        glDeleteBuffers(1, &m_ebo);
    }

    if (m_vbo)
    {
        glDeleteBuffers(1, &m_vbo);
    }

    if (m_vao)
    {
        glDeleteVertexArrays(1, &m_vao);
    }
}

bool Mesh::Create(const std::vector<float>&vertices, const std::vector<unsigned int>&indices)
{
	m_indexCount = static_cast<GLsizei>(indices.size()); // set the sie of indeices (e.g. number of triangles) 36 indices 12 triangles

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo); 

	glBindVertexArray(m_vao); 

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // This VBO is now the current vertex buffer.

    // copies our CPU vertex array into GPU-accessible memory.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo); // telling the renderer which vertices form which triangles.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    /*
        X Y Z | NormalX NormalY NormalZ
          3   +        3
              = 6 floats

    */

    // Attribute 0 reads position.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // Attribute 1 skips the first 3 floats and reads the normal.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return true;
}

void Mesh::Draw() const
{
	glBindVertexArray(m_vao);

	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
}
