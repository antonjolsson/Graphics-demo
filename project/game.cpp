//
// Created by Anton Olsson on 2020-02-20.
//

#include "game.h"


#include <glm/detail/_vectorize.hpp>
#include <glm/detail/_vectorize.hpp>
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
	if (debugGUI) debug_gui::showDebugGUI(gWindow, ship, camera, light, renderer);
    //gui->update(score);
}

GameObject* Game::initLight(const vec3 _position) {
    auto lightComponent = new LightComponent(true, inputHandler);
    light = new GameObject(_position, vec3(0),
        vec3(1));
    const auto renderer = new ModelRenderComponent(light, shaderProgram, sphereModel, mat4(1.f));
	renderComponents->push_back(renderer);
    light->addComponent(lightComponent);
    if (renderer!= nullptr) light->addComponent(renderer);
    lightComponent->addGameObject(light);
	renderer->addGameObject(light);
	light->setEnabled(true);
    gameObjects.insert(light);
    receivers.push_back(light);
    return light;
}

void Game::initLandingPad(const bool _showHitbox) {
	landingPad = new GameObject(vec3(0), vec3{0, 3 * M_PI / 4, 0}, vec3(1));
	const auto renderer = new ModelRenderComponent(landingPad, shaderProgram, landingPadModel, mat4(1.f));
	landingPad->addComponent(renderer);
	renderComponents->push_back(renderer);
	landingPad->setEnabled(true);
	gameObjects.insert(landingPad);
	receivers.push_back(landingPad);
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
	renderComponents->push_back(heightFieldComp);
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
	renderComponents->push_back(ship->getComponent<ModelRenderComponent>());
}

void Game::initRenderer(InputHandler* _engine, const bool _showHitbox, const int _winWidth, const int _winHeight, 
    std::vector<GameObject*>* _lights, GameObject* _background) {
    renderer = new Renderer(_engine, camera, renderComponents, _showHitbox, _winWidth, _winHeight, _lights, ship, 
        _background, landingPad);
    renderer->setRenderShadows(true);
	renderer->setShadowMapProgram(simpleShaderProgram);
}

Game::Game(InputHandler* _inputHandler, const bool _showHitbox, const int _winWidth, const int _winHeight, SDL_Window* _gWindow) {
	inputHandler = _inputHandler;
    showHitBox = _showHitbox;
    enabled = true;
	gWindow = _gWindow;

	renderComponents = new std::vector<RenderComponent*>();
    audioPlayer = new GameAudioPlayer();
    components.push_back(audioPlayer);

	initShaders();
    
	const auto background = initBackground();
    initTerrain(_showHitbox);
	initLandingPad(_showHitbox);
	initShip(_showHitbox);
    initEnemies(_inputHandler, _showHitbox);
	
    initCamera(_winWidth, _winHeight);
    //GameObject* light = initLight(ship->getTransform().position + SPOTLIGHT_POS_OFFSET);
	GameObject* light = initLight(SUN_POSITION);
    
	const auto lights = new std::vector<GameObject*> {light};
    initRenderer(_inputHandler, _showHitbox, _winWidth, _winHeight, lights, background);
}

GameAudioPlayer::GameAudioPlayer()
{
    //playMusic(backgroundMusic);
}

