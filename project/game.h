#pragma once

#include "gameObject.h"
#include "gui.h"
#include <GL/glew.h>
#include <SDL_audio.h>
#include "Ship.h"
#include "AudioComponent.h"
#include "Camera.h"
#include "CameraComponent.h"
#include "engine.h"
#include "Renderer.h"

//#include "fbo.h"

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
	
    const vec3 LIGHT_POS_OFFSET{ 0, 50.0f, -40.0 };

	const int ENV_ROUGHNESSES = 8;

    SDL_Color clearColor = {};

    Ship *ship{};
	
    std::set<GameObject*> gameObjects;
	
    Engine* engine = nullptr;

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
    GLuint heightfieldProgram{};

    GameAudioPlayer* audioPlayer;
    Renderer* renderer{};
    GameObject* camera{};
    SDL_Window* gWindow;
    bool debugGUI = true;

public:

    void initEnemies(Engine*_engine, bool debug);
  
    //void initGUI();
    void init() override;
	void readMessages() override;
	void destroy() override;
    bool isQuitting() const;

    void initShaders();
    void initTerrain(Engine* _engine, bool _showHitbox);
    GameObject* initBackground();
    void initCamera(const int _winWidth, const int _winHeight);
    void initShip(bool _showHitbox);
    void initRenderer(Engine * _engine, const bool _showHitbox, const int _winWidth, const int _winHeight, 
        std::vector<GameObject*>* _lights, GameObject * _background);
    void update(float _dt, int _windowWidth, int _windowHeight) override;
    GameObject* initLight();
    Game(Engine* _engine, bool _showHitbox, int _winWidth, int _winHeight);
    Game(Engine* _engine, const bool _showHitbox, const int _winWidth, const int _winHeight, SDL_Window* _gWindow);
};
