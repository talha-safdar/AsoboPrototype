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
	m_indexCount = static_cast<GLsizei>(indices.size());

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    return true;
}

void Mesh::Draw() const
{
	glBindVertexArray(m_vao);

	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
}
