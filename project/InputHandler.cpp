//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//#pragma ide diagnostic ignored "MemberFunctionCanBeStatic"
//
// Created by Anton Olsson on 2020-01-29.
//

#include "InputHandler.h"


#include <iostream>
#include <SDL.h>

#include "../external/SDL2_mixer/include/SDL_mixer.h"


bool InputHandler::init(const int _width, const int _height, const unsigned int _scaling) {

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }
    if (SDL_CreateWindowAndRenderer(_width, _height, SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
        return false;
    }
    /*if (TTF_Init() != 0) {
        SDL_Log("Unable to initialize SDL_ttf");
        return false;
    }*/
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) {
        return false;
    }
    initGameControllers();
    return true;
}

void InputHandler::initGameControllers() { //Check for joysticks
    if (SDL_NumJoysticks() >= 1) gameController = SDL_GameControllerOpen(0);
}

void InputHandler::destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    //TTF_CloseFont(text.font);
    //TTF_Quit();
    SDL_GameControllerClose(gameController);
    gameController = nullptr;
    Mix_CloseAudio();
    SDL_Quit();
}

InputHandler::KeyStatus& InputHandler::getKeyStatus() {
    SDL_Event e;

	//Reset toggle keys
	keys.toggleDebugGui = false;
	
    while (SDL_PollEvent(&e)) {
    	
        // check keyboard state (which keys are still pressed)
        const uint8_t* state = SDL_GetKeyboardState(nullptr);
    	//Player
        keys.left = state[SDL_SCANCODE_LEFT] ||
            SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT) != 0;
        keys.right = state[SDL_SCANCODE_RIGHT] ||
            SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) != 0;
        keys.forward = state[SDL_SCANCODE_UP] ||
            SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_DPAD_UP) != 0;
        keys.reverse = state[SDL_SCANCODE_DOWN] ||
            SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN) != 0;
        keys.up = state[SDL_SCANCODE_A] ||
            SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_A) != 0;
        keys.down = state[SDL_SCANCODE_D];
        keys.missile = state[SDL_SCANCODE_SPACE] ||
            SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_X) != 0;
        keys.machinegun = state[SDL_SCANCODE_S] ||
            SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_X) != 0;

    	//Camera
    	keys.lowerCamera = state[SDL_SCANCODE_Q];
    	keys.raiseCamera = state[SDL_SCANCODE_E];
    	keys.forwardCamera = state[SDL_SCANCODE_W];
    	keys.backwardCamera = state[SDL_SCANCODE_S];
    	
        keys.quit = (e.type == SDL_QUIT || state[SDL_SCANCODE_ESCAPE] ||
            SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_START) != 0);

    	keys.toggleDebugGui = state[SDL_SCANCODE_G];
    }
    return keys;
}

// Time in seconds
float InputHandler::getElapsedTime() {
    return (float) SDL_GetTicks() / 1000;
}

void InputHandler::quit() {
    destroy();
    exit(0);
}

void InputHandler::swapBuffers() const {
    SDL_RenderPresent(renderer);
}

void InputHandler::clearWindow(const SDL_Color _color) const {
    SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a);
    SDL_RenderClear(renderer);
}

// Useful for inputHandler extension
int InputHandler::getRandomInt(int _min, const int max) {
    std::mt19937 rng(randomSeed());
    std::uniform_int_distribution<int> uniformIntDist(_min, max - 1);
    return uniformIntDist(rng);
}

//#pragma clang diagnostic pop