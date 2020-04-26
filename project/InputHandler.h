#pragma once

#include <SDL_render.h>
#include <random>
#include <SDL_gamecontroller.h>
#include "vector2D.h"

#undef main


class InputHandler
{

    std::random_device randomSeed;

    //Game Controller 1 handler
    SDL_GameController* gameController = nullptr;

public:

    //Sprite *createColliderSprite();

    //Sprite *createSprite(const SpriteSheet& sheet);

	//enum TextAlign {NO_CHANGE, CENTER, LEFT};

	// Destroys the avancez library instance
	void destroy();

	// Destroys the avancez library instance and exits
	void quit();

	// Creates the main window. Returns true on success.
    bool init(int _width, int _height, unsigned int _scaling);

	// Clears the screen and draws all sprites and texts which have been drawn
	// since the last update call.
	// If update returns false, the application should terminate.

	void swapBuffers() const;

	void clearWindow(SDL_Color _color) const;

    // Return the total time spent in the game, in seconds.
	float getElapsedTime();

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

    int getRandomInt(int _min, int _max);

    //void drawText(int x, int y, const char *msg, TextAlign pos, SDL_Color color);

    //void setFont(const char *filePath, int fontSize, SDL_Color color);

    //Sprite *createSprite(const char *filePath);

private:

    SDL_Window * window{};
	SDL_Renderer * renderer{};

	KeyStatus keys{};

    void initGameControllers();

};

