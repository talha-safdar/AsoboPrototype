#pragma once

#include "../Graphics/Shader.h"

#include <iostream>
#include <glad/gl.h>
#include <SDL3/SDL.h>

/*
* It owns:
* SDL lifecycle
* Window
* OpenGL context
* Main loop
* 
* Destructor handles cleanup of all resources (RAII)
*/
class Application
{
public:
	Application();
	~Application();

	bool Initialize();
	void Run();

private:
	bool InitializeGraphics();
	void ProcessEvents();
	void Update();
	void Render();
	void Shutdown();

private:
	SDL_Window* m_window;
	SDL_GLContext m_glContext;

	Shader m_shader;

	GLuint m_vao = 0;
	GLuint m_vbo = 0;
	GLuint m_ebo = 0;
	//GLuint m_shaderProgram = 0;

	bool m_running;
};