#pragma once

#include <SDL_render.h>
#include <random>
#include <SDL_gamecontroller.h>
#include "vector2D.h"

#undef main

/*struct SpriteSheet {

    std::string filePath;
    SDL_Color colorKey;
};

class Sprite
{
	SDL_Renderer * renderer;
	SDL_Texture * texture;

public:

	Sprite(SDL_Renderer * renderer, SDL_Texture * texture, unsigned int scaling);

	// Destroys the sprite instance
	void destroy() const;

	// draw the sprite at the given position.
	// Valid coordinates are between (0,0) (upper left) and (width-32, height-32) (lower right).
	// (All sprites are 32*32 pixels, clipping is not supported)
    void draw(Vector2D screenPos, Vector2D sheetPos, Vector2D dimensions);
    void draw(Vector2D screenPos, Vector2D sheetPos, Vector2D srcDims, Vector2D destDims, bool xFlipped);

	void drawOutline(Vector2D screenPosition, Vector2D dimensions, SDL_Color color);

	static SDL_Rect getRect(const Vector2D &position, const Vector2D &dimensions);
};*/


class Engine
{
    const char* WINDOW_TITLE = "Mega Man";
    const char* ICON_FILE = "resource/icon.bmp";

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
	void processInput();

	void swapBuffers();

	void clearWindow(SDL_Color color);

    // Return the total time spent in the game, in seconds.
	float getElapsedTime();

	struct KeyStatus
	{
		bool fire; // a / gamepad a
        bool jump; // c / gamepad b
		bool left; // left arrow
		bool right; // right arrow
        bool up; // left arrow
        bool down; // right arrow
		bool quit; // escape button
	};

	// Returns the keyboard status. If a flag is set, the corresponding key is being held down.
    KeyStatus & getKeyStatus();

    int getRandomInt(int min, int max);

    //void drawText(int x, int y, const char *msg, TextAlign pos, SDL_Color color);

    //void setFont(const char *filePath, int fontSize, SDL_Color color);

    //Sprite *createSprite(const char *filePath);

private:

    SDL_Window * window{};
	SDL_Renderer * renderer{};

    /*struct Text {
        //TTF_Font* font;
        SDL_Surface* surface = nullptr;
        SDL_Texture* texture;
    };*/

    //Text text{};

	KeyStatus keys{};

    void initGameControllers();

};

