// AsoboPrototype.cpp : Defines the entry point for the application.
//

#include <glad/gl.h>
#include <SDL3/SDL.h>
#include "AsoboPrototype.h"

int main()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "SDL initialisation failed: "
			      << SDL_GetError()
				  << std::endl;

		return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// create window
	SDL_Window* window = SDL_CreateWindow("Asobo Prototype", 1280, 720, SDL_WINDOW_OPENGL);

	SDL_GLContext glContext = SDL_GL_CreateContext(window); // create OpenGL context for this window

	int version = gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress));
	if (version == 0)
	{
		std::cerr << "Failed to load OpenGL functions."
			<< std::endl;

		SDL_GL_DestroyContext(glContext);
		SDL_DestroyWindow(window);
		SDL_Quit();

		return -1;
	}

	std::cout
		<< "OpenGL "
		<< GLAD_VERSION_MAJOR(version)
		<< "."
		<< GLAD_VERSION_MINOR(version)
		<< std::endl;

	if (window == nullptr)
	{
		std::cerr << "window creation failed: "
			<< SDL_GetError()
			<< std::endl;

		SDL_Quit();
		return -1;
	}

	bool running = true;
	SDL_Event event;

	/*
	*	        (0, 0.5)
				   ▲
				  / \
				 /   \
		(-0.5,-0.5) (0.5,-0.5)
	*/
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

	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;

	// create vertex array object
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// bind vertex array object
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	// upload vertex data to GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
	GLuint shaderProgram = glCreateProgram();

	// attach shaders to program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// link shader program
	glLinkProgram(shaderProgram);

	// delete shaders as they are no longer needed after linking
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	while (running)
	{
		// input / events
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT) // if user closes the window
			{
				running = false;
			}
		}

		// rendering
		glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);

		// glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		SDL_GL_SwapWindow(window);
	}

	glDeleteProgram(shaderProgram);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &ebo);
	SDL_GL_DestroyContext(glContext); // destroy OpenGL context
	SDL_DestroyWindow(window);
	SDL_Quit();
}
