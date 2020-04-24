//
// Created by Anton Olsson on 2020-02-20.
//

#include "game.h"
#include "fbo.h"
#include <labhelper.h>
#include "CameraComponent.h"
#include "AudioComponent.h"
#include "engine.h"
#include "Renderer.h"

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

void Game::update(const float _dt, int _windowWidth, int _windowHeight) {
    readMessages();
    for (auto component : components) {
        component->update(_dt);
    }
    ship->update(_dt);
    for (auto gameObject : gameObjects)
        gameObject->update(_dt);
    camera->update(_dt, _windowWidth, _windowHeight);
    renderer->draw();
    //gui->update(score);
}

GameObject* Game::initLight() {
    auto lightComponent = new LightComponent(ship, LIGHT_POS_OFFSET);
	
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

void Game::readMessages() {
	for (Message m : mailbox) {
		switch (m) {
        case QUIT:
            quit = true;
            destroy();
			break;
        case PLAYER_KILLED :
            send(GAME_OVER);
            break;
        default:
            break;
		}
    }
}

bool Game::isQuitting() const {
    return quit;
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
    auto cameraComponent = new CameraComponent(ship);
    camera = new Camera();
    camera->addComponent(cameraComponent);
    cameraComponent->setCameraDirection(normalize(vec3(0.0f) - camera->getTransform().position));
    receivers.push_back(camera);
}

void Game::initShip(const bool _showHitbox) {
	ship = new Ship(engine, shaderProgram, _showHitbox);
    ship->addReceiver(this);
    gameObjects.insert(ship);
}

void Game::initRenderer(Engine* _engine, const bool _showHitbox, const int _winWidth, const int _winHeight) {
    std::vector<RenderComponent*> renderComponents;
    for (auto go : gameObjects) {
        auto renderComponent = go->getComponent<RenderComponent>();
        if (renderComponent != nullptr)
            renderComponents.push_back(renderComponent);
    }
    renderer = new Renderer(_engine, camera, renderComponents, _showHitbox, _winWidth, _winHeight);
    renderer->setRenderShadows(true);
}

Game::Game(Engine* _engine, bool _showHitbox, const int _winWidth, const int _winHeight) {
	engine = _engine;
    showHitBox = _showHitbox;
    enabled = true;

    audioPlayer = new GameAudioPlayer();
    components.push_back(audioPlayer);

    initCamera(_engine);
    GameObject* light = initLight();
    initShaders();

    initShip(_showHitbox);
    
    initTerrain(_engine, _showHitbox);
    initEnemies(_engine, _showHitbox);
    initBackground(_engine, _showHitbox);

    initRenderer(_engine, _showHitbox, _winWidth, _winHeight);
	
    receivers.push_back(ship);
}

GameAudioPlayer::GameAudioPlayer()
{
    //playMusic(backgroundMusic);
}

