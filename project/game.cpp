//
// Created by Anton Olsson on 2020-02-20.
//

#include "game.h"
#include "fbo.h"
#include <labhelper.h>
#include "camera.h"
#include "AudioPlayer.h"
#include "engine.h"

void Game::initEnemies(Engine*_engine, bool debug) {
    
}

/*void Game::initCrazyRazy(Engine *_engine, bool debug, int index) {
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

/*void Game::initGUI() {
    gui = new GUI(gameWidth, gameHeight);
    auto* render = new GUIRenderComponent();
    //render->create(gui, player, engine);
    gui->addComponent(render);
    receivers.push_back(gui);
    gui->enabled = true;
}*/

void Game::init() {
    //gui->init();
    enabled = true;
}

void Game::update(const float _dt) {
    const Engine::KeyStatus keys = engine->getKeyStatus();
    if (keys.quit) {
        destroy();
        engine->quit();
    }
    ship->update(_dt);
    for (auto gameObject : gameObjects)
        gameObject->update(_dt);
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
    ////delete gui;

    delete audioPlayer;
}

void Game::receive(Message _m) {
    switch (_m) {
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

void Game::initPlayer()
{
    
}

void Game::initShaders()
{
    backgroundProgram = labhelper::loadShaderProgram("../project/background.vert",
        "../project/background.frag");
    shaderProgram = labhelper::loadShaderProgram("../project/shading.vert", "../project/shading.frag");
    simpleShaderProgram = labhelper::loadShaderProgram("../project/simple.vert", "../project/simple.frag");
    particleProgram = labhelper::loadShaderProgram("../project/particle.vert", "../project/particle.frag");
    heightfieldProgram = labhelper::loadShaderProgram("../project/heightfield.vert", "../project/shading.frag");
}

void Game::initTerrain(Engine* _engine, const bool _showHitbox)
{
	
	
}

void Game::initBackground(Engine* _engine, const bool _showHitbox)
{
	
}

void Game::initCamera(Engine* _engine)
{
    //auto* camera = new Camera(_engine);
    receivers.push_back(new Camera(_engine));
}

Game::Game(Engine* _engine, const bool _showHitbox)
{
    engine = _engine;
    showHitBox = _showHitbox;
    enabled = true;

    audioPlayer = new GameAudioPlayer();
    components.push_back(audioPlayer);

    initCamera(_engine);
    initShaders();
    ship = new Ship(_engine, shaderProgram, _showHitbox);
    
    initTerrain(_engine, _showHitbox);
    initEnemies(_engine, _showHitbox);
    initBackground(_engine, _showHitbox);
	
    receivers.push_back(ship);
}

GameAudioPlayer::GameAudioPlayer()
{
    //playMusic(backgroundMusic);
}

