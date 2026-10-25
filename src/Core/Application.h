#pragma once

#include "../Graphics/Shader.h"
#include "../Graphics/Mesh.h"
#include "../Gameplay/Camera.h"
#include "../Gameplay/Enemy.h"
#include "../Physics/Collision.h"
#include "../Gameplay/Player.h"
#include "../Navigation/NavigationGrid.h"
#include "../Navigation/Pathfinder.h"

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

	//bool CheckCollisionXZ(const glm::vec3& aPosition, const glm::vec3& aHalfSize, const glm::vec3& bPosition, const glm::vec3& bHalfSize) const;

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
	Player m_player;
	NavigationGrid m_navigationGrid{ 21, 21, 1.0f };
	Pathfinder m_pathfinder;

	// unique_ptr is used to manage the lifetime of Shader and Mesh objects (ownership),
	// ensuring they are properly deleted when the Application object is destroyed.
	// Application is the single owner of these two
	// don't need to manually manage new/delete
	// but we need to reset (destroy the owned object) before SDL_GL_DestroyContext
	std::unique_ptr<Shader> m_shader;
	std::unique_ptr<Mesh> m_mesh;

	//glm::vec3 m_playerPosition{ 0.0f, 0.0f, 0.0f }; // Where does the player exist in the world?
	//glm::vec3 m_playerDirection{ 0.0f, 0.0f, -1.0f }; // which direction the player is facing.
	//float m_playerYaw = glm::pi<float>(); // left-right rotation
	//float m_verticalVelocity = 0.0f; // how fast vertically moving
	//bool m_isGrounded = true; // is standing on the floor

	float m_deltaTime;
	Uint64 m_lastFrameTime;

	GLuint m_vao = 0;
	GLuint m_vbo = 0;
	GLuint m_ebo = 0;
	//GLuint m_shaderProgram = 0;

	bool m_running;

	//glm::vec3 m_enemyPosition{ 0.0f, 0.0f, -10.0f };
	Enemy m_enemy{ glm::vec3(0.0f, 0.0f, -10.0f) };

	std::vector<AABB> m_obstacles
	{
		{ glm::vec3(4.0f, 0.0f, -5.0f), glm::vec3(0.5f) },
		{ glm::vec3(-4.0f, 0.0f, -8.0f), glm::vec3(0.5f) }
	};
};