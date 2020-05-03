//
// Created by Anton Olsson on 2020-02-20.
//

#include "game.h"
#include <iostream>
#include "fbo.h"
#include <labhelper.h>
#include "CameraComponent.h"
#include "AudioComponent.h"
#include "DebugGUI.h"
#include "InputHandler.h"
#include "EnvironmentComponent.h"
#include "hdr.h"
#include "LightComponent.h"
#include "Renderer.h"
#include "Camera.h"
#include "HeightFieldComp.h"

void Game::initEnemies(InputHandler*_engine, bool _debug) {
    
}

/*void Game::initCrazyRazy(InputHandler *_engine, bool debug, int index) {
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
    //render->create(gui, player, inputHandler);
    gui->addComponent(render);
    receivers.push_back(gui);
    gui->enabled = true;
}*/

void Game::init() {
    //gui->init();
    enabled = true;
}

void Game::update(const float _dt, const int _windowWidth, const int _windowHeight) {
    readMessages();
	if (quit) return;
	inputHandler->processInput(debugGUI);
    for (auto component : components) {
        component->update(_dt);
    }
    ship->update(_dt);
    for (auto gameObject : gameObjects)
        gameObject->update(_dt);
    camera->update(_dt, _windowWidth, _windowHeight);
    renderer->draw();
	if (debugGUI) debug_gui::showDebugGUI(gWindow, ship, camera);
    //gui->update(score);
}

GameObject* Game::initLight() {
    auto lightComponent = new LightComponent(true, inputHandler);
    auto light = new GameObject(ship->getTransform().position + LIGHT_POS_OFFSET, vec3(0),
        vec3(1));
    light->addComponent(lightComponent);
    lightComponent->addGameObject(light);
    gameObjects.insert(light);
    receivers.push_back(light);
    return light;
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
        case TOGGLE_DEBUG_GUI :
			debugGUI = !debugGUI;
			break;
        default:
            break;
		}
    }
	mailbox.clear();
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
   
}

void Game::initTerrain(const bool _showHitbox) {
	auto terrain = new GameObject();
	auto heightFieldComp = new HeightFieldComp(HEIGHTFIELD_PATH, TERRAIN_PHOTO_PATH, terrain);
	terrain->addComponent(heightFieldComp);
	gameObjects.insert(terrain);
	receivers.push_back(terrain);
}

GameObject* Game::initBackground()
{
	auto background = new GameObject();
	auto envComp = new EnvironmentComponent(background, backgroundProgram);
	background->addComponent(envComp);

	std::vector<std::string> filenames;
	filenames.reserve(ENV_ROUGHNESSES);
	for(auto i = 0; i < ENV_ROUGHNESSES; i++)
		filenames.push_back("../scenes/envmaps/" + envComp->getEnvmapBaseName() + "_dl_" + std::to_string(i) + ".hdr");

	envComp->setReflectionMap(labhelper::loadHdrMipmapTexture(filenames));
	envComp->setEnvironmentMap(labhelper::loadHdrTexture("../scenes/envmaps/" + envComp->getEnvmapBaseName() + ".hdr"));
	envComp->setIrradianceMap(labhelper::loadHdrTexture("../scenes/envmaps/" + envComp->getEnvmapBaseName() + "_irradiance.hdr"));

	gameObjects.insert(background);

	return background;
}

void Game::initCamera(const int _winWidth, const int _winHeight)
{
    auto cameraComponent = new CameraComponent(ship, _winWidth, _winHeight, inputHandler);
	cameraComponent->setTracingObject(false);
	cameraComponent->setMouseMovable(true);
    camera = new Camera();
    camera->addComponent(cameraComponent);
    cameraComponent->init(camera);
	gameObjects.insert(camera);
    receivers.push_back(camera);
}

void Game::initShip(const bool _showHitbox) {
	ship = new Ship(inputHandler, shaderProgram, _showHitbox);
    ship->addReceiver(this);
    gameObjects.insert(ship);
    receivers.push_back(ship);
}

void Game::initRenderer(InputHandler* _engine, const bool _showHitbox, const int _winWidth, const int _winHeight, 
    std::vector<GameObject*>* _lights, GameObject* _background) {
	auto* renderComponents = new std::vector<ModelRenderComponent*>();
    for (auto go : gameObjects) {
        auto renderComponent = go->getComponent<ModelRenderComponent>();
        if (renderComponent != nullptr)
            renderComponents->push_back(renderComponent);
    }
    renderer = new Renderer(_engine, camera, renderComponents, _showHitbox, _winWidth, _winHeight, _lights, ship, 
        _background);
    renderer->setRenderShadows(true);
	renderer->setShadowMapProgram(simpleShaderProgram);
}

Game::Game(InputHandler* _engine, const bool _showHitbox, const int _winWidth, const int _winHeight, SDL_Window* _gWindow) {
	inputHandler = _engine;
    showHitBox = _showHitbox;
    enabled = true;
	gWindow = _gWindow;
	
    audioPlayer = new GameAudioPlayer();
    components.push_back(audioPlayer);

	initShaders();
    initShip(_showHitbox);
    
    initTerrain(_showHitbox);
    initEnemies(_engine, _showHitbox);
    auto background = initBackground();
	
    initCamera(_winWidth, _winHeight);
    GameObject* light = initLight();
    
	const auto lights = new std::vector<GameObject*> {light};
    initRenderer(_engine, _showHitbox, _winWidth, _winHeight, lights, background);
	
}

GameAudioPlayer::GameAudioPlayer()
{
    //playMusic(backgroundMusic);
}

