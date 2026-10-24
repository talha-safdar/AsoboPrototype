#pragma once

#include <glad/gl.h>
#include <string>

/*
* It owns OpenGL shader program
* Destructor deletes it
*/
class Shader
{
public:
	Shader();
	~Shader();

	bool Create(const std::string& vertexSource, const std::string& fragmentSource);
	void Bind() const;

private:
	bool CompileShader(GLuint shader, const std::string& source, const char* shaderName) const;

private:
	GLuint m_program;
};