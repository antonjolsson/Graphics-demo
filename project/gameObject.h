#pragma once

// GameObject represents objects which moves are drawn

#include <vector>
#include <map>
#include "component.h"
#include "../external/SDL2_mixer/include/SDL_mixer.h"

enum Message {
    ENEMY_HIT, GAME_OVER, LEVEL_WON, QUIT, FIRE, PLAYER_KILLED, ICE_HIT, GROUND_HIT, WATER_HIT,
        OUT_OF_WATER, PLAYER_HIT, OBJ_IN_AIR, MOVED_LEFT, MOVED_RIGHT, NO_INPUT, BULLET_HIT, MSG_EXPLODED, CRAZY_KILLED,
        HIT_BY_CRAZY, HIT_FROM_LEFT, HIT_FROM_RIGHT, HIT_FROM_TOP, HIT_FROM_BOTTOM, PEPE_KILLED, HIT_BY_PEPE};

enum Tag {PLAYER, ENEMY, GROUND, WATER, BULLET, NONE, ICE, TRAPDOOR, CRAZY_RAZY, PEPE};

enum Mode {IDLE, IN_AIR, IN_AIR_FIRING, RUNNING, RUNNING_FIRING, FIRING, EXPLODING_MODE, TAKEN_DAMAGE, FLYING};

class Component;

class GameObject
{
protected:

    const char* ENEMY_HIT_SOUND = "resource/sounds/EnemyDamage.wav";
    const int DEFAULT_EFFECT_VOL = SDL_MIX_MAXVOLUME / 2;
    const int HIT_SOUND_VOL = SDL_MIX_MAXVOLUME / 3;

	std::vector<GameObject*> receivers;
	std::vector<Component*> components;

    Vector2D viewPortPosition {0, 0};

    Mix_Chunk* sound;

	unsigned int levelWidth = 0;

    int attackDamage = 0;
    bool invincibleAfterHit = false;
    int healthPoints = 0;

    bool gameOver = false;
    bool levelWon = false;

public:

    bool isGameOver() const;

    bool isLevelWon() const;

	unsigned int getLevelWidth() const;

    std::map<Tag, Message> objectHitMessageMap {
            {PLAYER, PLAYER_HIT},
            {ENEMY, ENEMY_HIT},
            {GROUND, GROUND_HIT},
            {ICE, ICE_HIT},
            {WATER, WATER_HIT},
            {BULLET, BULLET_HIT},
            {CRAZY_RAZY, HIT_BY_CRAZY},
    };

    // Refactor to use setters/getters?

    Vector2D worldPosition {0, 0};
    Vector2D screenPosition {0, 0};

    Vector2D acceleration {0, 0};
    Vector2D velocity {0, 0};

    Vector2D spriteDims {0, 0};

	bool facingRight = false;

    Tag type = NONE;
    Mode mode = IDLE;

    bool inAir = false;
    bool inWater = false;

	unsigned int gameWidth = 0;

    bool enabled = false;

    bool destroyedForever = false;

	virtual ~GameObject();

	virtual void create();
	virtual void addComponent(Component* component);
	virtual void destroy();
	virtual void addReceiver(GameObject *go);
	virtual void receive(Message m);
	virtual void send(Message m);
    void sendComponents(Message message);

    virtual void decreaseHP(int damage){};

    virtual void create(unsigned int _gameWidth);

    template<typename T>
    T getComponent() {
        for (Component * c : components) {
            T t = dynamic_cast<T>(c);  //ugly but it works...
            if (t != nullptr) {
                return t;
            }
        }
        return nullptr;
    }

    Vector2D worldToScreenPos(Vector2D viewPortPos);

	void setLevelWidth(unsigned int _levelWidth);

    int getAttackDamage();

    bool isInvincibleAfterHit() const;

    void playSound(const char *path, int volume);

    void addComponents(const std::vector<Component *> &_components);

    void setPosition(Vector2D viewPortPos);

    virtual void init();
    void init(int hp, int _attackDamage);

    virtual void update(float dt, Vector2D viewPortPos);
    void update(float dt, const Vector2D &viewPortPos, int initialHP, int _attackDamage);

    void create(unsigned int _gameWidth, Tag _type, Mode _mode, Vector2D worldPos);
};