#pragma once

#include "gameObject.h"
#include "gui.h"
#include <GL/glew.h>
#include <SDL_audio.h>
#include "Ship.h"
#include "AudioComponent.h"
#include "InputHandler.h"
#include "Renderer.h"

using namespace glm;

class GameAudioPlayer : public AudioComponent
{

    Audio backgroundMusic{ "../project/resource/AirwolfExtendedVersion.mp3", SDL_MIX_MAXVOLUME / 4,
    	true };

public:

    GameAudioPlayer();
	
};

class Game : public GameObject {

    const unsigned int MAX_NUM_GAME_OBJECTS = 10000;
    const float OPT_FRAMERATE = 60.f;
    const vec3 SPOTLIGHT_POS_OFFSET{ 0, 50.0f, -40.0 };

	const int ENV_ROUGHNESSES = 8;

	const std::string HEIGHTFIELD_PATH = "../scenes/nlsFinland/L3123F.png";
	const std::string TERRAIN_PHOTO_PATH = "../scenes/nlsFinland/L3123F_downscaled.jpg";

	labhelper::Model* sphereModel = labhelper::loadModelFromOBJ("../scenes/sphere.obj");

    SDL_Color clearColor = {};

    Ship *ship{};
	
    std::set<GameObject*> gameObjects;
	
    InputHandler* inputHandler = nullptr;

    ObjectPool<GameObject>* playerCollObjects{};
    ObjectPool<GameObject>* enemyCollObjects{};
    ObjectPool<GameObject>* bulletCollObjects{};
    std::set<GameObject*> bgCollObjects;

    //GUI *gui = nullptr;

	std::map<Message, int> eventScoreMap {};

	unsigned int score = 0;

	bool quit = false;
    bool showHitBox;

    GLuint shaderProgram{};       // Shader for rendering the final image
    GLuint simpleShaderProgram{}; // Shader used to draw the shadow map
    GLuint simpleParticleProgram{};
    GLuint particleProgram{};
    GLuint backgroundProgram{};

    GameAudioPlayer* audioPlayer;
    Renderer* renderer{};
    GameObject* camera{};
    SDL_Window* gWindow;
    bool debugGUI = true;
    vec3 SUN_POSITION {160, 1120, -3000};
    GameObject* sun;

public:

    void initEnemies(InputHandler*_engine, bool _debug);
  
    //void initGUI();
    void init() override;
	void readMessages() override;
	void destroy() override;
    bool isQuitting() const;

    void initShaders();
    void initTerrain(bool _showHitbox);
    GameObject* initBackground();
    void initCamera(int _winWidth, int _winHeight);
    void initShip(bool _showHitbox);
    void initRenderer(InputHandler * _engine, bool _showHitbox, int _winWidth, int _winHeight, 
        std::vector<GameObject*>* _lights, GameObject * _background);
    void update(float _dt, int _windowWidth, int _windowHeight) override;
    GameObject* initLight(glm::vec3 _position);
    Game(InputHandler* _engine, bool _showHitbox, int _winWidth, int _winHeight, SDL_Window* _gWindow);
};
