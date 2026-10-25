#include "Application.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

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

	m_running = true;
	return true;
}

bool Application::InitializeGraphics()
{
	std::vector<float> vertices =
	{
		// Back
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		// Front
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		// Left
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,

		// Right
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,

		 // Bottom
		 -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,

		 // Top
		 -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		  0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f
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
		// 12 triangles / 36 indices.
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20
	};

	glEnable(GL_DEPTH_TEST); // Depth testing stops far-away surfaces from incorrectly drawing over nearer ones.

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

		out vec3 vNormal;

		uniform mat4 uModel;
		uniform mat4 uView;
		uniform mat4 uProjection;

		void main()
		{
			gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
			vNormal = mat3(transpose(inverse(uModel))) * aNormal;
		}
	)";

	// fragment shader
	// dot() = How much is this surface facing the light?
	const char* fragmentShaderSource = R"(
		#version 460 core

		in vec3 vNormal;

		out vec4 FragColor;

		uniform vec3 uColor;
		uniform vec3 uLightDirection;

		void main()
		{
			vec3 normal = normalize(vNormal);
			vec3 lightDirection = normalize(uLightDirection);

			float diffuse = max(dot(normal, lightDirection), 0.0);
			float ambient = 0.25;
			float brightness = ambient + diffuse * 0.75;

			FragColor = vec4(uColor * brightness, 1.0);
		}
	)";

	if (!m_shader->Create(vertexShaderSource, fragmentShaderSource))
	{
		return false;
	}

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
	const float movementSpeed = 3.0f;
	const float movementAmount = movementSpeed * m_deltaTime;

	glm::vec3 previousPosition = m_playerPosition; // Player just moved inside a wall? then restore

	glm::vec3 forward = m_camera.GetForward();
	glm::vec3 right = m_camera.GetRight();
	glm::vec3 movement(0.0f);

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

	const float gravity = -9.81f;
	const float jumpVelocity = 5.5f;

	if (keyboardState[SDL_SCANCODE_SPACE] && m_isGrounded)
	{
		m_verticalVelocity = jumpVelocity;
		m_isGrounded = false;
	}

	m_verticalVelocity += gravity * m_deltaTime; // Gravity continuously changes vertical velocity downward.
	m_playerPosition.y += m_verticalVelocity * m_deltaTime; // actually moves the player vertically.

	if (m_playerPosition.y <= 0.0f)
	{
		m_playerPosition.y = 0.0f;
		m_verticalVelocity = 0.0f;
		m_isGrounded = true;
	}

	if (glm::length(movement) > 0.0f)
	{
		movement = glm::normalize(movement);
		m_playerDirection = movement;

		float targetYaw = atan2(m_playerDirection.x, m_playerDirection.z);
		float angleDifference = std::remainder(targetYaw - m_playerYaw, glm::two_pi<float>());
		float rotationAmount = std::clamp(10.0f * m_deltaTime, 0.0f, 1.0f);

		m_playerYaw += angleDifference * rotationAmount;

		float cosYaw = std::abs(std::cos(m_playerYaw));
		float sinYaw = std::abs(std::sin(m_playerYaw));

		float halfX = (0.3f * cosYaw) + (0.65f * sinYaw);
		float halfZ = (0.3f * sinYaw) + (0.65f * cosYaw);

		glm::vec3 playerHalfSize(halfX, 0.5f, halfZ);
		glm::vec3 obstacleHalfSize(0.5f, 0.5f, 0.5f);

		m_playerPosition.x += movement.x * movementAmount;

		bool hitGreenX = CheckCollisionXZ(m_playerPosition, playerHalfSize, m_greenCubePosition, obstacleHalfSize);
		bool hitBlueX = CheckCollisionXZ(m_playerPosition, playerHalfSize, m_blueCubePosition, obstacleHalfSize);

		if (hitGreenX || hitBlueX)
		{
			m_playerPosition.x = previousPosition.x;
		}

		m_playerPosition.z += movement.z * movementAmount;

		bool hitGreenZ = CheckCollisionXZ(m_playerPosition, playerHalfSize, m_greenCubePosition, obstacleHalfSize);
		bool hitBlueZ = CheckCollisionXZ(m_playerPosition, playerHalfSize, m_blueCubePosition, obstacleHalfSize);

		if (hitGreenZ || hitBlueZ)
		{
			m_playerPosition.z = previousPosition.z;
		}
	}

	glm::vec3 playerHalfSize(0.3f, 0.5f, 0.65f);
	glm::vec3 obstacleHalfSize(0.5f, 0.5f, 0.5f);

	bool hitGreen = CheckCollisionXZ(m_playerPosition, playerHalfSize, m_greenCubePosition, obstacleHalfSize);
	bool hitBlue = CheckCollisionXZ(m_playerPosition, playerHalfSize, m_blueCubePosition, obstacleHalfSize);

	if (hitGreen || hitBlue)
	{
		// undo the movement
		m_playerPosition.x = previousPosition.x;
		m_playerPosition.z = previousPosition.z;
	}

	m_camera.Update(m_playerPosition);

	glm::vec3 target = m_playerPosition + glm::vec3(0.0f, 0.5f, 0.0f);
	glm::vec3 desiredCameraPosition = m_camera.GetPosition();

	float greenHitDistance = 0.0f;
	float blueHitDistance = 0.0f;

	bool hitGreen2 = LineIntersectsAABB(target, desiredCameraPosition, m_greenCubePosition, obstacleHalfSize, greenHitDistance);
	bool hitBlue2 = LineIntersectsAABB(target, desiredCameraPosition, m_blueCubePosition, obstacleHalfSize, blueHitDistance);

	float closestHit = glm::length(desiredCameraPosition - target);
	bool cameraBlocked = false;

	if (hitGreen2 && greenHitDistance < closestHit)
	{
		closestHit = greenHitDistance;
		cameraBlocked = true;
	}

	if (hitBlue2 && blueHitDistance < closestHit)
	{
		closestHit = blueHitDistance;
		cameraBlocked = true;
	}

	if (cameraBlocked)
	{
		glm::vec3 direction = glm::normalize(desiredCameraPosition - target);
		m_camera.SetPosition(target + direction * (closestHit - 0.2f));
	}
}

void Application::Render()
{
	glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear depth buffer and the colour buffer

	m_shader->Bind(); // Use our shader program.

	m_shader->SetVec3("uLightDirection", glm::vec3(-0.5f, 1.0f, 0.3f)); // Light is coming from above and slightly from one side.

	// camera
	glm::mat4 view = m_camera.GetViewMatrix(m_playerPosition); // camera needs to know what is following
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f); // FOV, Aspect, Near plane, Far plane
	m_shader->SetMat4("uView", view);
	m_shader->SetMat4("uProjection", projection);

	// player
	glm::mat4 playerModel = glm::mat4(1.0f);
	playerModel = glm::translate(playerModel, m_playerPosition);
	playerModel = glm::rotate(playerModel, m_playerYaw, glm::vec3(0.0f, 1.0f, 0.0f));
	playerModel = glm::scale(playerModel, glm::vec3(0.6f, 1.0f, 1.3f));
	m_shader->SetMat4("uModel", playerModel);
	m_shader->SetVec3("uColor", glm::vec3(1.0f, 0.5f, 0.2f));
	m_mesh->Draw();

	// floor
	glm::mat4 floorModel = glm::mat4(1.0f);
	floorModel = glm::translate(floorModel, glm::vec3(0.0f, -0.55f, 0.0f));
	floorModel = glm::scale(floorModel, glm::vec3(20.0f, 0.1f, 20.0f));
	m_shader->SetMat4("uModel", floorModel);
	m_shader->SetVec3("uColor", glm::vec3(0.25f, 0.25f, 0.25f));
	m_mesh->Draw();

	// green box
	glm::mat4 objectModel = glm::mat4(1.0f);
	objectModel = glm::translate(objectModel, m_greenCubePosition);
	m_shader->SetMat4("uModel", objectModel);
	m_shader->SetVec3("uColor", glm::vec3(0.2f, 0.7f, 0.3f));
	m_mesh->Draw();

	// blue box
	objectModel = glm::mat4(1.0f);
	objectModel = glm::translate(objectModel, m_blueCubePosition);
	m_shader->SetMat4("uModel", objectModel);
	m_shader->SetVec3("uColor", glm::vec3(0.3f, 0.4f, 0.9f));
	m_mesh->Draw();

	SDL_GL_SwapWindow(m_window);
}

bool Application::CheckCollisionXZ(const glm::vec3& aPosition, const glm::vec3& aHalfSize, const glm::vec3& bPosition, const glm::vec3& bHalfSize) const
{
	/*
		Are they overlapping on X?
		AND
		Are they overlapping on Z?

		Yes + Yes = collision
	*/
	bool overlapX = std::abs(aPosition.x - bPosition.x) < (aHalfSize.x + bHalfSize.x);
	bool overlapZ = std::abs(aPosition.z - bPosition.z) < (aHalfSize.z + bHalfSize.z);

	return overlapX && overlapZ;
}

bool Application::LineIntersectsAABB(const glm::vec3& start, const glm::vec3& end, const glm::vec3& boxPosition, const glm::vec3& boxHalfSize, float& hitDistance) const
{
	glm::vec3 direction = end - start;
	float length = glm::length(direction);

	if (length <= 0.0001f)
		return false;

	direction = glm::normalize(direction);

	glm::vec3 boxMin = boxPosition - boxHalfSize;
	glm::vec3 boxMax = boxPosition + boxHalfSize;

	float tMin = 0.0f;
	float tMax = length;

	for (int axis = 0; axis < 3; ++axis)
	{
		if (std::abs(direction[axis]) < 0.0001f)
		{
			if (start[axis] < boxMin[axis] || start[axis] > boxMax[axis])
				return false;

			continue;
		}

		float t1 = (boxMin[axis] - start[axis]) / direction[axis];
		float t2 = (boxMax[axis] - start[axis]) / direction[axis];

		if (t1 > t2)
			std::swap(t1, t2);

		tMin = std::max(tMin, t1);
		tMax = std::min(tMax, t2);

		if (tMin > tMax)
			return false;
	}

	hitDistance = tMin;
	return true;
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