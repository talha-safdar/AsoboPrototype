#include "Application.h"

Application::Application() : m_window(nullptr), m_glContext(nullptr), m_running(false), m_vao(0), m_vbo(0), m_ebo(0), m_shaderProgram(0)
{
    
}

Application::~Application()
{
	Shutdown();
}

bool Application::Initialize()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "SDL initialisation failed: "
			<< SDL_GetError()
			<< std::endl;

		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// create window
	m_window = SDL_CreateWindow("Asobo Prototype", 1280, 720, SDL_WINDOW_OPENGL);

	m_glContext = SDL_GL_CreateContext(m_window); // create OpenGL context for this window

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
	float vertices[] =
	{
		//0.0f,  0.5f, 0.0f, // top vertex
		//-0.5f, -0.5f, 0.0f, // bottom left vertex
		//0.5f, -0.5f, 0.0f  // bottom right vertex

			// x      y      z
		 0.5f,  0.5f,  0.0f,   // 0 top-right
		 0.5f, -0.5f,  0.0f,   // 1 bottom-right
		-0.5f, -0.5f,  0.0f,   // 2 bottom-left
		-0.5f,  0.5f,  0.0f    // 3 top-left
	};

	unsigned int indices[] =
	{
		0, 1, 3,
		1, 2, 3
	};

	// VBO = actual vertex data
	// VAO = remembers how OpenGL should interpret that vertex data
	// Shader = tells GPU how to process/draw it
	// VBO = vertex information
	// EBO = which vertices form the triangles
	// VAO = remembers the vertex / index configuration

	// create vertex array object
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	// bind vertex array object
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// upload vertex data to GPU


	// specify vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	// enable vertex attribute
	glEnableVertexAttribArray(0);

	// vertex shader
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

	// compile vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// attach shader source code to shader object
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

	// compile vertex shader
	glCompileShader(vertexShader);

	// compile fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// attach shader source code to shader object
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

	// compile fragment shader
	glCompileShader(fragmentShader);

	// create shader program
	m_shaderProgram = glCreateProgram();

	// attach shaders to program
	glAttachShader(m_shaderProgram, vertexShader);
	glAttachShader(m_shaderProgram, fragmentShader);

	// link shader program
	glLinkProgram(m_shaderProgram);

	// delete shaders as they are no longer needed after linking
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

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
{}

void Application::Render()
{
	// rendering
	glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(m_shaderProgram);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	SDL_GL_SwapWindow(m_window);
}

void Application::Shutdown()
{
	m_running = false;

	if (m_shaderProgram)
	{
		glDeleteProgram(m_shaderProgram);
		m_shaderProgram = 0;
	}

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