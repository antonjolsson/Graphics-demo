//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//#pragma ide diagnostic ignored "MemberFunctionCanBeStatic"

#include "InputHandler.h"
#include <iostream>
#include <SDL.h>
#include "../external/SDL2_mixer/include/SDL_mixer.h"
#include "imgui_impl_sdl_gl3.h"
#include <imgui_impl_sdl_gl3.cpp>

void InputHandler::initGameControllers() { //Check for joysticks
    if (SDL_NumJoysticks() >= 1) gameController = SDL_GameControllerOpen(0);
}

void InputHandler::destroy() {
    SDL_GameControllerClose(gameController);
    gameController = nullptr;
    SDL_Quit();
}

InputHandler::KeyStatus& InputHandler::getKeyStatus() {
    return keys;
}

void InputHandler::checkPressedKeys() {
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
}

void InputHandler::resetInput() {
	keys.toggleDebugGui = false;
	keys.quit = false;

	mouse.deltaX = 0;
	mouse.deltaY = 0;
}

void InputHandler::initMouseDrag(const SDL_Event _event) {
	if (_event.button.button == SDL_BUTTON_LEFT)
		mouse.isDragging = true;
	else if (_event.button.button == SDL_BUTTON_RIGHT)
		mouse.isMouseRightDragging = true;
	int x; int y;
	SDL_GetMouseState(&x, &y);
	mouse.prevMouseCoords.x = x;
	mouse.prevMouseCoords.y = y;
}

void InputHandler::setMouseMovement(const SDL_Event _event) {
	// More info at https://wiki.libsdl.org/SDL_MouseMotionEvent
	mouse.deltaX = _event.motion.x - mouse.prevMouseCoords.x;
	mouse.deltaY = _event.motion.y - mouse.prevMouseCoords.y;
	mouse.prevMouseCoords.x = _event.motion.x;
	mouse.prevMouseCoords.y = _event.motion.y;
}

void InputHandler::processInput(bool _showDebugGUI) {
	SDL_Event event;

	resetInput();

	// Allow ImGui to capture events.
	ImGuiIO& io = ImGui::GetIO();
    while (SDL_PollEvent(&event)) {
    	
    	ImGui_ImplSdlGL3_ProcessEvent(&event);

		if (event.type == SDL_QUIT || (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE))
			keys.quit = true;
		if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_g)
			keys.toggleDebugGui = true;
    	else if (event.type == SDL_MOUSEBUTTONDOWN && (!_showDebugGUI || !ImGui::GetIO().WantCaptureMouse)
			&& (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT)
			&& !(mouse.isDragging || mouse.isMouseRightDragging)) {
			initMouseDrag(event);
		}

		if (!(SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
			mouse.isDragging = false;
		if (!(SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_RIGHT)))
			mouse.isMouseRightDragging = false;
		if (event.type == SDL_MOUSEMOTION)
			setMouseMovement(event);
    }
	if (!io.WantCaptureKeyboard)
			checkPressedKeys();
}

void InputHandler::quit() {
    destroy();
    exit(0);
}

InputHandler::MouseStatus& InputHandler::getMouseStatus() {
	return mouse;
}

// TODO: move
int InputHandler::getRandomInt(int _min, const int max) {
    std::mt19937 rng(randomSeed());
    std::uniform_int_distribution<int> uniformIntDist(_min, max - 1);
    return uniformIntDist(rng);
}

//#pragma clang diagnostic pop