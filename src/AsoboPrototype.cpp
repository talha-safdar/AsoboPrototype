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

	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT) // if user closes the window
			{
				running = false;
			}

			glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			SDL_GL_SwapWindow(window);
		}
	}

	SDL_GL_DestroyContext(glContext); // destroy OpenGL context
	SDL_DestroyWindow(window);
	SDL_Quit();
}
