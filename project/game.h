#pragma once

#include "gameObject.h"
#include "gui.h"
#include <GL/glew.h>
#include <SDL_audio.h>
#include "ship.h"
#include "../external/SDL2_mixer/include/SDL_mixer.h"

//#include "fbo.h"

class Game : public GameObject {

    const unsigned int MAX_NUM_GAME_OBJECTS = 10000;

    const char* MUSIC_FILE = "../project/resource/AirwolfExtendedVersion.mp3";
    const int MUSIC_VOLUME = SDL_MIX_MAXVOLUME / 4;

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

    Mix_Music* music = nullptr;

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

    void playMusic();

    void initShip();

    void initShaders();
    Game(AvancezLib* _engine, const bool _showHitbox);
};
