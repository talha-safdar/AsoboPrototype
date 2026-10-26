#include "Application.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <algorithm>
#include <cmath>

Application::Application() : m_window(nullptr), m_glContext(nullptr), m_running(false), m_vao(0), m_vbo(0), m_ebo(0), m_deltaTime(0.0f), m_lastFrameTime(0)
{
    
}

Application::~Application()
{
	Shutdown();
}

bool Application::Initialize()
{
	// necesary for window, OpenGL context, and video-related events
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "SDL initialisation failed: "
			<< SDL_GetError()
			<< std::endl;

		return false;
	}

	// ### configure OpenGL context attributes ###
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

	/*
	* ask for modern core profile
	* VAOs
	* VBOs
	* Shaders
	* modern programmable rendering
	*/
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	/*
	* enable double buffering
	* Front Buffer → what monitor currently shows
	* Back Buffer  → frame we are currently drawing
	*/
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // asks for a 24-bit depth buffer for 3D
	// ### configure OpenGL context attributes ###

	/*
		SDL Window
		   │
		   └── physical place where pixels appear

		OpenGL Context
		   │
		   └── OpenGL state/resources/environment used to render those pixels
	*/

	// SDL Window: create window (SDL asks windows to create a native window
	// SDL_WINDOW_OPENGL menas is going to be used for OpenGL rendering
	m_window = SDL_CreateWindow("Asobo Prototype", 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

	// OpenGL Context: create OpenGL context for this window
	m_glContext = SDL_GL_CreateContext(m_window);

	// GLAD is useful to retrievve opengl functions in memory (it loads opengl functions) while SDL can find them
	int version = gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress));
	if (version == 0)
	{
		std::cerr << "Failed to load OpenGL functions."
			<< std::endl;

		SDL_GL_DestroyContext(m_glContext);
		SDL_DestroyWindow(m_window);
		SDL_Quit();

		return false;
	}

	if (!SDL_SetWindowRelativeMouseMode(m_window, true))
	{
		std::cerr << "Failed to enable relative mouse mode: " << SDL_GetError() << std::endl;
		return false;
	}

	if (!SDL_SetWindowMouseGrab(m_window, true))
	{
		std::cerr << "Failed to grab mouse: " << SDL_GetError() << std::endl;
		return false;
	}

	if (!InitializeGraphics())
	{
		return false;
	}

	m_navigationGrid.Build(m_obstacles); // needs rebuilding when the navigation obstacles change.

	std::vector<glm::vec3> testPath = m_pathfinder.FindPath(m_navigationGrid, glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	//std::cout << "Path waypoints: " << testPath.size() << std::endl;

	//for (const glm::vec3& point : testPath)
	//{
		// std::cout << "Waypoint: " << point.x << ", " << point.z << std::endl;
	//}


	glm::ivec2 greenCell = m_navigationGrid.WorldToCell(m_obstacles[0].position);
	glm::ivec2 emptyCell = m_navigationGrid.WorldToCell(glm::vec3(0.0f, 0.0f, 0.0f));

	m_running = true;
	return true;
}

bool Application::InitializeGraphics()
{
	m_texture = std::make_unique<Texture>();

	if (!m_texture->Load("assets/textures/cursed-island.png"))
	{
		return false;
	}

	m_testModel = std::make_unique<Model>();

	if (!m_testModel->Load("assets/models/CarConcept.glb"))
	{
		return false;
	}

	std::vector<Vertex> vertices =
	{
		// Back
		{{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
		 {{0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
		 {{0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},

		// Front
		{{-0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},

		// Left
		{{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},

		// Right
		 {{0.5f, -0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
		 {{0.5f, -0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
		 {{0.5f,  0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
		 {{0.5f,  0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},

		 // Bottom
		 {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
		  {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
		  {{0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
		 {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},

		 // Top
		 {{-0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
		  {{0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
		  {{0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
		 {{-0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
	};

	// 2 triangles, for GPU 2 draw 2 triangles which form a rectangle
	/*
		Triangle 1
		0 → 1 → 3

		Triangle 2
		1 → 2 → 3
		3 -------- 0
		| \        |
		|   \      |
		|     \    |
		2 -------- 1
	*/
	std::vector<unsigned int> indices =
	{
		// Back (-Z)
		0, 2, 1,
		0, 3, 2,

		// Front (+Z)
		4, 5, 6,
		4, 6, 7,

		// Left (-X)
		8, 9, 10,
		8, 10, 11,

		// Right (+X)
		12, 14, 13,
		12, 15, 14,

		// Bottom (-Y)
		16, 17, 18,
		16, 18, 19,

		// Top (+Y)
		20, 22, 21,
		20, 23, 22
	};

	glEnable(GL_DEPTH_TEST); // Depth testing stops far-away surfaces from incorrectly drawing over nearer ones.
	glEnable(GL_CULL_FACE); // enables face removal.
	glCullFace(GL_BACK); // Don't draw triangle faces pointing away from the camera.
	glFrontFace(GL_CCW); // Counter-clockwise vertex order represents the front of a triangle.

	// Dynamically create a Shader and give exclusive ownership to Application
	m_mesh = std::make_unique<Mesh>(); // application owns the mesh
	m_shader = std::make_unique<Shader>();

	if (!m_mesh->Create(vertices, indices))
	{
		return false;
	}

	// VBO = actual vertex data
	// VAO = remembers how OpenGL should interpret that vertex data
	// Shader = tells GPU how to process/draw it
	// VBO = vertex information
	// EBO = which vertices form the triangles
	// VAO = remembers the vertex / index configuration

	// specify vertex attributes
	// 0: layout(location = 0) in vec3 aPosition;
	// 3: each vertex has 3 componenets x y z
	// GL_FLOAT: each component is a float
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr); 

	// enable vertex attribute
	glEnableVertexAttribArray(0); // enables shader input location 0

	// vertex shader (GLSL code)
	// vNormal = Transform the normal correctly when the object rotates or scales.
	const char* vertexShaderSource = R"(
		#version 460 core

		layout(location = 0) in vec3 aPosition;
		layout(location = 1) in vec3 aNormal;
		layout(location = 2) in vec2 aTexCoord;

		out vec3 vNormal;
		out vec2 vTexCoord;

		uniform mat4 uModel;
		uniform mat4 uView;
		uniform mat4 uProjection;

		void main()
		{
			gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
			vNormal = mat3(transpose(inverse(uModel))) * aNormal;
			vTexCoord = aTexCoord;
		}
	)";

	// fragment shader
	// dot() = How much is this surface facing the light?
	const char* fragmentShaderSource = R"(
		#version 460 core

		in vec3 vNormal;
		in vec2 vTexCoord;

		out vec4 FragColor;

		uniform vec3 uColor;
		uniform vec3 uLightDirection;
		uniform sampler2D uTexture;
		uniform int uUseTexture;

		void main()
		{
			vec3 normal = normalize(vNormal);
			vec3 lightDirection = normalize(uLightDirection);

			float diffuse = max(dot(normal, lightDirection), 0.0);
			float ambient = 0.25;
			float brightness = ambient + diffuse * 0.75;

			vec3 baseColor = uColor;

			if (uUseTexture == 1)
			{
				// Read the pixel from the texture at this UV position.
				baseColor *= texture(uTexture, vTexCoord).rgb;
			}

			FragColor = vec4(baseColor * brightness, 1.0);
		}
	)";

	if (!m_shader->Create(vertexShaderSource, fragmentShaderSource))
	{
		return false;
	}

	if (!m_testModel->Load("assets/models/CarConcept.glb"))
	{
		return false;
	}

	m_testModel->PrintParts();

	return true;
}

void Application::Run()
{
	m_lastFrameTime = SDL_GetTicksNS();

	while (m_running)
	{
		Uint64 currentTime = SDL_GetTicksNS();
		m_deltaTime = static_cast<float>(currentTime - m_lastFrameTime) / 1'000'000'000.0f; // nanoseconds (1s)
		m_lastFrameTime = currentTime;

		ProcessEvents();
		Update();
		Render();
	}
}

void Application::ProcessEvents()
{
	/*
		Mouse moved
		Keyboard pressed
		Window resized
		Window closed
		Controller input
	*/
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT) // if user closes the window
		{
			m_running = false;
		}

		if (event.type == SDL_EVENT_MOUSE_MOTION)
		{
			/*
				xrel = how far mouse moved horizontally
				yrel = how far mouse moved vertically
			*/
			m_camera.ProcessMouseMovement(event.motion.xrel, event.motion.yrel);
		}
	}
}

void Application::Update()
{
	const bool* keyboardState = SDL_GetKeyboardState(nullptr);
	glm::vec3 movement(0.0f);
	glm::vec3 forward = m_camera.GetForward();
	glm::vec3 right = m_camera.GetRight();

	if (keyboardState[SDL_SCANCODE_W])
	{
		movement += forward;
	}

	if (keyboardState[SDL_SCANCODE_S])
	{
		movement -= forward;
	}

	if (keyboardState[SDL_SCANCODE_D])
	{
		movement += right;
	}

	if (keyboardState[SDL_SCANCODE_A])
	{
		movement -= right;
	}

	bool jumpPressed = keyboardState[SDL_SCANCODE_SPACE];

	// update player
	m_player.Update(movement, jumpPressed, m_deltaTime, m_obstacles);

	// update enemy
	bool enemyAttacked = m_enemy.Update(m_player.GetPosition(), m_deltaTime, m_obstacles, m_navigationGrid, m_pathfinder);

	if (enemyAttacked)
	{
		m_player.TakeDamage(10);
		std::cout << "Player health: " << m_player.GetHealth() << std::endl;
	}

	// update camera
	m_camera.Update(m_player.GetPosition());
}

void Application::Render()
{
	// Shader = HOW to render
	// Mesh = WHAT geometry exists
	// Model matrix = WHERE that geometry exists

	/*
		Mesh = geometry/data
		Shader = instructions for processing that data
		Material = values/textures supplied to those instructions
	*/

	glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear depth buffer and the colour buffer

	m_shader->Bind(); // Use our shader program.
	m_texture->Bind(0); // image bint to slot 0
	m_shader->SetInt("uTexture", 0);

	// They are extra values the shader needs for the current draw.
	m_shader->SetVec3("uLightDirection", glm::vec3(-0.5f, 1.0f, 0.3f)); // Light is coming from above and slightly from one side.

	// camera
	glm::mat4 view = m_camera.GetViewMatrix(m_player.GetPosition()); // camera needs to know what is following
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f); // FOV, Aspect, Near plane, Far plane
	m_shader->SetMat4("uView", view);
	m_shader->SetMat4("uProjection", projection);

	// player
	glm::mat4 playerModel = glm::mat4(1.0f);
	playerModel = glm::translate(playerModel, m_player.GetPosition());
	playerModel = glm::rotate(playerModel, m_player.GetYaw(), glm::vec3(0.0f, 1.0f, 0.0f));
	playerModel = glm::scale(playerModel, glm::vec3(0.6f, 1.0f, 1.3f));
	m_shader->SetMat4("uModel", playerModel);
	m_shader->SetInt("uUseTexture", 1); // 1 textured uploaded
	m_shader->SetVec3("uColor", glm::vec3(1.0f));
	m_mesh->Draw(); // GPU, use this mesh configuration and draw triangles using these indices.

	// floor
	glm::mat4 floorModel = glm::mat4(1.0f);
	floorModel = glm::translate(floorModel, glm::vec3(0.0f, -0.55f, 0.0f));
	floorModel = glm::scale(floorModel, glm::vec3(20.0f, 0.1f, 20.0f));
	m_shader->SetMat4("uModel", floorModel);
	m_shader->SetVec3("uColor", glm::vec3(0.25f, 0.25f, 0.25f));
	m_shader->SetInt("uUseTexture", 0); // no texture to apply
	m_mesh->Draw();

	// green box
	glm::mat4 objectModel = glm::mat4(1.0f);
	objectModel = glm::translate(objectModel, m_obstacles[0].position);
	m_shader->SetMat4("uModel", objectModel);
	m_shader->SetVec3("uColor", glm::vec3(0.2f, 0.7f, 0.3f));
	m_shader->SetInt("uUseTexture", 1);
	m_mesh->Draw();

	// blue box
	objectModel = glm::mat4(1.0f);
	objectModel = glm::translate(objectModel, m_obstacles[1].position);
	m_shader->SetMat4("uModel", objectModel);
	m_shader->SetVec3("uColor", glm::vec3(0.3f, 0.4f, 0.9f));
	m_shader->SetInt("uUseTexture", 0);
	m_mesh->Draw();

	// moving enemy
	glm::mat4 enemyModel = glm::mat4(1.0f);
	enemyModel = glm::translate(enemyModel, m_enemy.GetPosition());
	enemyModel = glm::rotate(enemyModel, m_enemy.GetYaw(), glm::vec3(0.0f, 1.0f, 0.0f));
	enemyModel = glm::scale(enemyModel, glm::vec3(0.7f, 1.0f, 1.2f));
	glm::vec3 enemyColor;
	switch (m_enemy.GetState())
	{
		case Enemy::State::Idle:
			enemyColor = glm::vec3(0.5f, 0.1f, 0.1f);
			break;

		case Enemy::State::Chase:
			enemyColor = glm::vec3(1.0f, 0.5f, 0.0f);
			break;
		case Enemy::State::Search:
			enemyColor = glm::vec3(0.2f, 0.4f, 1.0f);
			break;
		case Enemy::State::Attack:
			enemyColor = glm::vec3(0.0f, 1.0f, 0.0f);
			break;
	}
	m_shader->SetMat4("uModel", enemyModel);
	m_shader->SetVec3("uColor", enemyColor);
	m_shader->SetInt("uUseTexture", 0); // 1: apply texture, 0: do not apply texture
	m_mesh->Draw();

	// draw waypoints of pathfinding
	const std::vector<glm::vec3>& enemyPath = m_enemy.GetPath();

	for (const glm::vec3& waypoint : enemyPath)
	{
		glm::mat4 waypointModel = glm::mat4(1.0f);
		waypointModel = glm::translate(waypointModel, waypoint + glm::vec3(0.0f, 0.05f, 0.0f));
		waypointModel = glm::scale(waypointModel, glm::vec3(0.15f, 0.05f, 0.15f));

		m_shader->SetMat4("uModel", waypointModel);
		m_shader->SetVec3("uColor", glm::vec3(0.1f, 0.4f, 1.0f));
		m_mesh->Draw();
	}

	// 3d model
	glm::mat4 importedModel = glm::mat4(1.0f);
	importedModel = glm::translate(importedModel, glm::vec3(2.0f, 0.0f, 2.0f));
	m_shader->SetVec3("uColor", glm::vec3(0.8f, 0.8f, 0.8f));
	m_shader->SetInt("uUseTexture", 0);
	//m_testModel->RotatePart("WheelFrontRRim", glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	m_testModel->Draw(*m_shader, importedModel);

	SDL_GL_SwapWindow(m_window);
}

void Application::Shutdown()
{
	/*
	Game stops
	↓
	Mesh dies
	↓
	Shader dies
	↓
	OpenGL context dies
	↓
	Window dies
	↓
	SDL dies
	*/

	m_running = false;

	/*
	* the reason why we use "." is becasue we are talking to unique_ptr,
	* we only use "->" when we want to access memebrs of the owneed object :)
	* 
	* m_shader
	   │
	   │ unique_ptr functions
	   ├── .reset()
	   ├── .get()
	   │
	   └── points to
			 ↓
		   Shader
			 ├── ->Bind()
			 └── ->Create()
	*/
	m_testModel.reset();
	m_texture.reset();
	m_mesh.reset(); // delete shader program
	m_shader.reset(); // delete shader program

	if (m_ebo)
	{
		glDeleteBuffers(1, &m_ebo);
		m_ebo = 0;
	}

	if (m_vbo)
	{
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}

	if (m_vao)
	{
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}

	if (m_glContext)
	{
		SDL_GL_DestroyContext(m_glContext);
		m_glContext = nullptr;
	}

	if (m_window)
	{
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}

	SDL_Quit();
}