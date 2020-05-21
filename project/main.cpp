
#include "game.h"
#include "HeightFieldComp.h"
#ifdef _WIN32
extern "C" _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
#endif

#include <GL/glew.h>
#include <cstdlib>
#include <chrono>
#include <iostream>

#include <labhelper.h>

#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>

using namespace glm;

#include <Model.h>
#include "main.h"

const bool SHOW_HITBOX = false;
const std::string CAPTION = "Foreign Attack";
const int WIN_WIDTH = 1920;
const int WIN_HEIGHT = 1080;

SDL_Window* g_window = nullptr;
static float currentTime = 0.0f;
static float deltaTime = 0.0f;
bool showUI = true;
bool logger = false;
int antiAliasSamples = 16;
const float OPT_FRAMERATE = 60.f;
float previousTime = 0.0f;
int windowWidth, windowHeight;

void setWindowSize() {
	int w, h;
	SDL_GetWindowSize(g_window, &w, &h);
	if(w != windowWidth || h != windowHeight)
	{
		windowWidth = w;
		windowHeight = h;
	}
}

void logStats() {
	
}

int main(int argc, char* argv[])
{
	
	g_window = labhelper::init_window_SDL("Foreign Attack", WIN_WIDTH, WIN_HEIGHT, antiAliasSamples);

	InputHandler inputHandler;

	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	
	Game game(&inputHandler, SHOW_HITBOX, WIN_WIDTH, WIN_HEIGHT, g_window);

	auto quit = false;
	const auto startTime = std::chrono::system_clock::now();

	while(!game.isQuitting())
	{
		setWindowSize();
		//update currentTime
		std::chrono::duration<float> timeSinceStart = std::chrono::system_clock::now() - startTime;
		previousTime = currentTime;
		currentTime = timeSinceStart.count();
		deltaTime = currentTime - previousTime;

		game.update(deltaTime, windowWidth, windowHeight);
	
		if (logger) logStats();

		// Swap front and back buffer. This frame will now been displayed.
		SDL_GL_SwapWindow(g_window);

	}

	// Shut down everything. This includes the window and all other subsystems.
	labhelper::shutDown(g_window);

	Mix_CloseAudio();
	return 0;
}
