#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>

/*
* It owns OpenGL shader program
* Destructor deletes it
*/
class Shader
{
public:
	Shader();

	// RAII cleanup.
	~Shader();

	/*
	* compiles vertex shader
	* compiles fragment shader
	* checks errors
	* links them into a program
	*/
	bool Create(const std::string& vertexSource, const std::string& fragmentSource);

	/*
	* makes that shader program active for rendering.
	*/
	void Bind() const;
	void SetMat4(const std::string& name, const glm::mat4& value) const;

	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetInt(const std::string& name, int value) const;

private:
	bool CompileShader(GLuint shader, const std::string& source, const char* shaderName) const;

private:
	GLuint m_program;
};