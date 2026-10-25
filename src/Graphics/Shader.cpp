#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Shader::Shader() : m_program(0)
{}

Shader::~Shader()
{
	if (m_program)
	{
		glDeleteProgram(m_program);
		m_program = 0;
	}
}

bool Shader::CompileShader(GLuint shader, const std::string& source, const char* shaderName) const
{
	const char* sourcePtr = source.c_str();

	glShaderSource(shader, 1, &sourcePtr, nullptr);
	glCompileShader(shader);

	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		char infoLog[512];

		glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);

		std::cerr << shaderName << " compilation failed:\n" << infoLog << std::endl;

		return false;
	}

	return true;
}

bool Shader::Create(const std::string& vertexSource, const std::string& fragmentSource)
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    if (!CompileShader(vertexShader, vertexSource, "Vertex shader"))
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    if (!CompileShader(fragmentShader, fragmentSource, "Fragment shader"))
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    m_program = glCreateProgram();

    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);

    glLinkProgram(m_program);

    // delete temporary shader objects after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[1024];

        glGetProgramInfoLog(m_program, sizeof(infoLog), nullptr, infoLog);

        std::cerr
            << "Shader program linking failed:\n"
            << infoLog
            << std::endl;

        glDeleteProgram(m_program);
        m_program = 0;

        return false;
    }

    return true;
}

void Shader::Bind() const
{
    // For the things I'm about to draw, use this vertex/fragment shader program.
    // that program stays active until you bind another one.
    glUseProgram(m_program);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
{
    // A uniform is basically data we send to the shader that stays the same for the whole draw call.
    GLint location = glGetUniformLocation(m_program, name.c_str());

    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    GLint location = glGetUniformLocation(m_program, name.c_str());

    glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
}
