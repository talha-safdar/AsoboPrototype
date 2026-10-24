// AsoboPrototype.cpp : Defines the entry point for the application.
//

#include "AsoboPrototype.h"
#include <SDL3/SDL.h>

int main()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "SDL initialisation failed: "
			      << SDL_GetError()
				  << std::endl;

		return -1;
	}

	SDL_Window* window = SDL_CreateWindow("Asobo Prototype", 1280, 720, SDL_WINDOW_OPENGL);

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
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
}
