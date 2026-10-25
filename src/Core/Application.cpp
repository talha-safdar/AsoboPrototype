#include "Application.h"

Application::Application() : m_window(nullptr), m_glContext(nullptr), m_running(false), m_vao(0), m_vbo(0), m_ebo(0)
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
	m_window = SDL_CreateWindow("Asobo Prototype", 1280, 720, SDL_WINDOW_OPENGL);

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
		// x      y      z
		 0.5f,  0.5f,  0.0f,   // 0 top-right
		 0.5f, -0.5f,  0.0f,   // 1 bottom-right
		-0.5f, -0.5f,  0.0f,   // 2 bottom-left
		-0.5f,  0.5f,  0.0f    // 3 top-left
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
		0, 1, 3,
		1, 2, 3
	};

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr); 

	// enable vertex attribute
	glEnableVertexAttribArray(0); // enables shader input location 0

	// vertex shader (GLSL code)
	const char* vertexShaderSource = R"(
		#version 460 core

		layout(location = 0) in vec3 aPosition;

		void main()
		{
			gl_Position = vec4(aPosition, 1.0);
		}
	)";

	// fragment shader
	const char* fragmentShaderSource = R"(
		#version 460 core

		out vec4 FragColor;

		void main()
		{
			FragColor = vec4(1.0, 0.5, 0.2, 1.0);
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
	while (m_running)
	{
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
	}
}

void Application::Update()
{

}

void Application::Render()
{
	glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_shader->Bind(); // Use our shader program.
	m_mesh->Draw(); // Use the index buffer and draw triangles.

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