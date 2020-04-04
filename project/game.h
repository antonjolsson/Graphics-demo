#pragma once

#include "gameObject.h"
#include "gui.h"
#include <GL/glew.h>
#include <SDL_audio.h>
#include "ship.h"
#include "AudioPlayer.h"

//#include "fbo.h"

class GameAudioPlayer : public AudioPlayer
{

    Audio backgroundMusic{ "../project/resource/AirwolfExtendedVersion.mp3", SDL_MIX_MAXVOLUME / 4, true };

public:

    GameAudioPlayer();
};

class Game : public GameObject {

    const unsigned int MAX_NUM_GAME_OBJECTS = 10000;

    unsigned int gameWidth = 0;
    unsigned int gameHeight = 0;
    unsigned int levelWidth = 0;

    SDL_Color clearColor = {};

    Ship* ship{};
	
    std::set<GameObject*> gameObjects;
	
	AvancezLib* engine = nullptr;

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

public:

    void initEnemies(AvancezLib *_engine, bool debug);
  
    virtual void init(unsigned int _gameWidth, unsigned int _gameHeight, AvancezLib* newEngine, SDL_Color _clearColor,
            bool debug);
    //void initGUI();
    void initPlayer();
    void init() override;
	void update(float _dt) override;
	virtual void draw();
	void receive(Message _m) override;
	void destroy() override;
    bool isQuitting() const;

    void initShip();

    void initShaders();
    Game(AvancezLib* _engine, const bool _showHitbox);
};
