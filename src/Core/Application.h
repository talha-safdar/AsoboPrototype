#pragma once

#include "../Graphics/Shader.h"
#include "../Graphics/Mesh.h"
#include "../Gameplay/Camera.h"

#include <iostream>
#include <glad/gl.h>
#include <memory>
#include <SDL3/SDL.h>
#include <glm/gtc/constants.hpp>

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

	bool CheckCollisionXZ(const glm::vec3& aPosition, const glm::vec3& aHalfSize, const glm::vec3& bPosition, const glm::vec3& bHalfSize) const;

	// Does the line between player and camera pass through this box ?
	bool LineIntersectsAABB(const glm::vec3& start, const glm::vec3& end, const glm::vec3& boxPosition, const glm::vec3& boxHalfSize, float& hitDistance) const;

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

	glm::vec3 m_playerPosition{ 0.0f, 0.0f, 0.0f }; // Where does the player exist in the world?
	glm::vec3 m_playerDirection{ 0.0f, 0.0f, -1.0f }; // which direction the player is facing.
	float m_playerYaw = glm::pi<float>(); // left-right rotation
	float m_verticalVelocity = 0.0f; // how fast vertically moving
	bool m_isGrounded = true; // is standing on the floor

	glm::vec3 m_greenCubePosition{ 4.0f, 0.0f, -5.0f };
	glm::vec3 m_blueCubePosition{ -4.0f, 0.0f, -8.0f };

	float m_deltaTime;
	Uint64 m_lastFrameTime;

	GLuint m_vao = 0;
	GLuint m_vbo = 0;
	GLuint m_ebo = 0;
	//GLuint m_shaderProgram = 0;

	bool m_running;
};