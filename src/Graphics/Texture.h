#pragma once

#include <glad/gl.h>
#include <string>

class Texture
{
public:
	Texture();
	~Texture();

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	bool Load(const std::string& path);
	void Bind(unsigned int slot = 0) const;

private:
	GLuint m_id;
};