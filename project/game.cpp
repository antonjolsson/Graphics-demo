//
// Created by Anton Olsson on 2020-02-20.
//

#include "game.h"

void Game::initEnemies(AvancezLib *_engine, bool debug) {
    
}

/*void Game::initCrazyRazy(AvancezLib *_engine, bool debug, int index) {
    auto* crazyRazy = new CrazyRazy();
    crazyRazy->create(gameWidth, crazyRazy->TYPE, crazyRazy->INITIAL_MODE, crazyRazy->getStartPos(index));
    gameObjects.insert(crazyRazy);
    receivers.push_back(crazyRazy);
    crazyRazy->addReceiver(this);
    playerCollObjects->add(crazyRazy);

    auto* behaviour = new CrazyBehaviourComponent();
    behaviour->create(_engine, crazyRazy, player, &gameObjects);
    auto* crazyRender = new CrazyRenderComponent();
    crazyRender->create(_engine, crazyRazy);
    auto* collider = new BoxCollideComponent2D();
    collider->create(_engine, crazyRazy, &gameObjects, enemyCollObjects, debug);
    crazyRazy->addComponents({behaviour, crazyRender, collider});

    bgCollObjects.insert(crazyRazy);
}*/

void Game::init(unsigned int _gameWidth, unsigned int _gameHeight, AvancezLib *newEngine, const SDL_Color _clearColor,
        bool debug) {
    gameWidth = _gameWidth;
    gameHeight = _gameHeight;
    engine = newEngine;
    clearColor = _clearColor;

    playerCollObjects = new ObjectPool<GameObject>();
    enemyCollObjects = new ObjectPool<GameObject>();
    bulletCollObjects = new ObjectPool<GameObject>();

    initEnemies(newEngine, debug);
    //initGUI();

    enabled = true;
}

/*void Game::initGUI() {
    gui = new GUI(gameWidth, gameHeight);
    auto* render = new GUIRenderComponent();
    //render->create(gui, player, engine);
    gui->addComponent(render);
    receivers.push_back(gui);
    gui->enabled = true;
}*/

void Game::initPlayer(AvancezLib *newEngine, bool debug) {
}

void Game::init() {
    //gui->init();
    enabled = true;
}

void Game::update(float dt) {
    const AvancezLib::KeyStatus keys = engine->getKeyStatus();
    if (keys.quit) {
        destroy();
        engine->quit();
    }
    for (auto gameObject : gameObjects)
        gameObject->update(dt);
    //gui->update(score);
}

void Game::draw() {
    engine->swapBuffers();
    engine->clearWindow(clearColor);
}

void Game::destroy() {
    for (auto gameObject : gameObjects)
        gameObject->destroy();

    delete playerCollObjects;
    delete enemyCollObjects;
    delete bulletCollObjects;
    //delete gui;

    music = nullptr;
}

void Game::receive(Message m) {
    switch (m) {
        case PLAYER_KILLED :
            send(GAME_OVER);
            break;
        default:
            break;
    }
}

bool Game::isQuitting() const {
    return quit;
}

void Game::playMusic() {
    Mix_VolumeMusic(MUSIC_VOLUME);
    music = Mix_LoadMUS(MUSIC_FILE);
    Mix_PlayMusic( music, -1 );
}

