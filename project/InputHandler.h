#pragma once

#include <glm/vec2.hpp>
#include <random>
#include <SDL_events.h>
#include <SDL_gamecontroller.h>

#undef main

class InputHandler
{
    std::random_device randomSeed;

    //Game Controller 1 handler
    SDL_GameController* gameController = nullptr;

public:

	void destroy();
	void quit();

    void checkPressedKeys();
    void resetInput();
    void initMouseDrag(SDL_Event _event);
    void setMouseMovement(SDL_Event _event);
    void processInput(bool _showDebugGUI);

    struct KeyStatus
	{
		bool missile; // space / gamepad a
        bool up; // a / gamepad b
		bool left; // left arrow
		bool right; // right arrow
        bool forward; // up arrow
        bool reverse; // down arrow
		bool quit; // escape button
		bool machinegun; // s
		bool down; // d
		
		bool lowerCamera; // q
		bool raiseCamera; // e
		bool forwardCamera; // w
		bool backwardCamera; // s TODO: find better name

		bool toggleDebugGui; // g
	};

	// Returns the keyboard status. If a flag is set, the corresponding key is being held down.
    KeyStatus & getKeyStatus();

    struct MouseStatus {
	    glm::ivec2 prevMouseCoords = { -1, -1 };
		bool isDragging = false;
		bool isMouseRightDragging = false;
    	int deltaX = 0;
    	int deltaY = 0;
    };

	MouseStatus& getMouseStatus();

    int getRandomInt(int _min, int _max);

private:

	KeyStatus keys{};
	MouseStatus mouse{};

    void initGameControllers();

};

