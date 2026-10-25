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

    // create IDs for GPU-side objects
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo); // VBO = vertex information
    glGenBuffers(1, &m_ebo); // EBO = which vertices form triangles

	glBindVertexArray(m_vao); 

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // This VBO is now the current vertex buffer. uploads the actual vertex data.

    // copies our CPU vertex array into GPU-accessible memory.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // uploads the indices.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo); // telling the renderer which vertices form which triangles.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    /*
        X Y Z | NormalX NormalY NormalZ
          3   +        3
              = 6 floats

    */

    // vertex sahder
    // layout(location = 0) in vec3 aPosition;
    // layout(location = 1) in vec3 aNormal;

    // Attribute 0: position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // Attribute 1: normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Attribute 2: UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // VBO memory
    // ↓
    // VAO layout
    // ↓
    //shader inputs

    return true;
}

void Mesh::Draw() const
{
	glBindVertexArray(m_vao);

	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
}
