#pragma once

#include "../Graphics/Shader.h"
#include "../Graphics/Mesh.h"
#include "../Gameplay/Camera.h"

#include <iostream>
#include <glad/gl.h>
#include <memory>
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

	/*
	* SDL initialization
	* OpenGL settings
	* Window creation
	* OpenGL context creation
	* GLAD loading
	* 
	* then it calls InitializeGraphics()
	*/
	bool Initialize();

	/*
	* ProcessEvents()
	* Update()
	* Render()
	*/
	void Run();

private:

	/*
	* Vertex/index data
	* Shader
	* Mesh
	* then also models, textures
	*/
	bool InitializeGraphics();

	/*
	* close window
	* keyboard
	* mouse
	* controller later
	* 
	* to exit the main loop, set m_running to false
	*/
	void ProcessEvents();

	/*
	* Player movement
	* Camera
	* AI
	* Physics
	* Animation
	* Gameplay
	*/
	void Update();

	/*
	* clear screen
	* bind shader
	* draw mesh
	* swap buffers
	* 
	* m_shader->Bind();
	* m_mesh->Draw();
	*/
	void Render();

	/*
		stop loop
		↓
		Mesh
		↓
		Shader
		↓
		OpenGL context
		↓
		Window
		↓
		SDL
	*/
	void Shutdown();

private:
	SDL_Window* m_window;
	SDL_GLContext m_glContext;
	Camera m_camera;

	// unique_ptr is used to manage the lifetime of Shader and Mesh objects (ownership),
	// ensuring they are properly deleted when the Application object is destroyed.
	// Application is the single owner of these two
	// don't need to manually manage new/delete
	// but we need to reset (destroy the owned object) before SDL_GL_DestroyContext
	std::unique_ptr<Shader> m_shader;
	std::unique_ptr<Mesh> m_mesh;

	float m_deltaTime;
	Uint64 m_lastFrameTime;

	GLuint m_vao = 0;
	GLuint m_vbo = 0;
	GLuint m_ebo = 0;
	//GLuint m_shaderProgram = 0;

	bool m_running;
};