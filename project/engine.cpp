//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//#pragma ide diagnostic ignored "MemberFunctionCanBeStatic"
//
// Created by Anton Olsson on 2020-01-29.
//

#include "engine.h"

#include <SDL.h>

#include "../external/SDL2_mixer/include/SDL_mixer.h"


bool Engine::init(const int _width, const int _height, const unsigned int _scaling) {

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
    SDL_SetWindowTitle(window, WINDOW_TITLE);
    SDL_SetWindowIcon(window, SDL_LoadBMP(ICON_FILE));
    initGameControllers();
    return true;
}

void Engine::initGameControllers() { //Check for joysticks
    if (SDL_NumJoysticks() >= 1) gameController = SDL_GameControllerOpen(0);
}

void Engine::destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    //TTF_CloseFont(text.font);
    //TTF_Quit();
    SDL_GameControllerClose(gameController);
    gameController = nullptr;
    Mix_CloseAudio();
    SDL_Quit();
}

Engine::KeyStatus& Engine::getKeyStatus() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        // check keyboard state (which keys are still pressed)
        const uint8_t* state = SDL_GetKeyboardState(nullptr);
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
        keys.quit = (e.type == SDL_QUIT || state[SDL_SCANCODE_Q] || state[SDL_SCANCODE_ESCAPE] ||
            SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_START) != 0);
    }
    return keys;
}

// Time in seconds
float Engine::getElapsedTime() {
    return (float) SDL_GetTicks() / 1000;
}

/*Sprite *Engine::createSprite(const char *filePath) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, filePath);
    return new Sprite(renderer, texture, scaling);;
}

Sprite *Engine::createColliderSprite() {
    return new Sprite(renderer, nullptr, scaling);
}*/

void Engine::quit() {
    destroy();
    exit(0);
}

void Engine::swapBuffers() {
    SDL_RenderPresent(renderer);
}

void Engine::clearWindow(const SDL_Color _color) {
    SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a);
    SDL_RenderClear(renderer);
}

// Useful for engine extension
int Engine::getRandomInt(int min, int max) {
    std::mt19937 rng(randomSeed());
    std::uniform_int_distribution<int> uniformIntDist(min, max - 1);
    return uniformIntDist(rng);
}

/*Sprite::Sprite(SDL_Renderer *renderer, SDL_Texture *texture, unsigned int _scaling) {
    this->renderer = renderer;
    this->texture = texture;
}

void Sprite::draw(Vector2D screenPos, Vector2D sheetPos, const Vector2D dimensions) {
    draw(screenPos, sheetPos, dimensions, dimensions, false);
}

void Sprite::draw(Vector2D screenPos, Vector2D sheetPos, Vector2D srcDims, Vector2D destDims, bool xFlipped) {
    SDL_Rect srcRect = {(int)round(sheetPos.x), (int)round(sheetPos.y), (int)round(srcDims.x), (int)round(srcDims.y)};
    SDL_Rect dstRect = getRect(screenPos, destDims);
    SDL_RenderCopyEx(renderer, texture, &srcRect, &dstRect, 0, nullptr,
            xFlipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void Engine::drawText(int x, int y, const char *msg, TextAlign pos, SDL_Color color) {
    //text.surface = TTF_RenderText_Solid(text.font, msg, color);
    text.texture = SDL_CreateTextureFromSurface(renderer, text.surface);
    int texW, texH = 0;
    SDL_QueryTexture(text.texture, nullptr, nullptr, &texW, &texH);
    if (pos == CENTER)
        x -= texW / 2;
    else if (pos == LEFT)
        x -= texW;
    y -= texH;
    SDL_Rect dstRect = Sprite::getRect({static_cast<double>(x), static_cast<double>(y)},
                                       {static_cast<double>(texW), static_cast<double>(texH)});
    SDL_RenderCopy(renderer, text.texture, nullptr, &dstRect);
}

void Engine::setFont(const char *filePath, int fontSize, const SDL_Color color) {
    //text.font = TTF_OpenFont(filePath, fontSize);
}*/

/*Sprite *Engine::createSprite(const SpriteSheet& sheet) {
    SDL_Surface* surface = IMG_Load(sheet.filePath.c_str());
    if (!surface) {
        printf("IMG_Load: %s\n", IMG_GetError());
        // handle error
    }
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, sheet.colorKey.r, sheet.colorKey.g,
            sheet.colorKey.b));
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return new Sprite(renderer, texture, scaling);
}*/

/*void Sprite::destroy() const {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
}

void Sprite::drawOutline(Vector2D screenPosition, Vector2D dimensions, SDL_Color color) {
    SDL_Rect outline = getRect(screenPosition, dimensions);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(renderer, &outline);
}

SDL_Rect Sprite::getRect(const Vector2D &position, const Vector2D &dimensions) {
    return {(int) round(position.x), (int) round(position.y),
            (int) round(dimensions.x), (int) round(dimensions.y)};
}*/

//#pragma clang diagnostic pop