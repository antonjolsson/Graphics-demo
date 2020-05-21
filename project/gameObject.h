#pragma once

#include <glm/detail/type_vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include <map>
#include "component.h"
#include <SDL.h>
#include "../external/SDL2_mixer/include/SDL_mixer.h"

enum Message {
    ENEMY_HIT, GAME_OVER, LEVEL_WON, QUIT, FIRE, PLAYER_KILLED, ICE_HIT, GROUND_HIT, WATER_HIT,
        OUT_OF_WATER, PLAYER_HIT, OBJ_IN_AIR, MOVED_LEFT, MOVED_RIGHT, NO_INPUT, BULLET_HIT, MSG_EXPLODED,
        HIT_FROM_LEFT, HIT_FROM_RIGHT, HIT_FROM_TOP, HIT_FROM_BOTTOM, TOGGLE_DEBUG_GUI};

enum Mode {IDLE, IN_AIR, IN_AIR_FIRING, RUNNING, RUNNING_FIRING, FIRING, EXPLODING_MODE, TAKEN_DAMAGE, FLYING};

class Component;

class GameObject
{
protected:

	struct Transform
	{
		glm::vec3 position { 0 };
        glm::vec3 rotation { 0 };
        glm::vec3 scale { 1 };
	};

	Transform transform;

    bool enabled = false;
	glm::mat4 modelMatrix;

public:
	void setEnabled(bool _enabled);
	glm::mat4& getModelMatrix();
protected:
	std::vector<GameObject*> receivers;
	std::vector<Component*> components;

    bool gameOver = false;
    bool levelWon = false;
	
	std::set<Message> mailbox;

public:
	const glm::vec3 X_AXIS{ 1.0f, 0.0f, 0.0f };
	const glm::vec3 Y_AXIS{ 0.0f, 1.0f, 0.0f };
	const glm::vec3 Z_AXIS{ 0.0f, 0.0f, 1.0f };
	
	glm::vec3 getPosition() const;
	glm::vec3 getRotation() const;
	glm::vec3 getScale() const;
	
	void setPosition(glm::vec3 _position);

	void setRotation(const glm::vec3 _rotation);

	void setScale(const glm::vec3 _scale);

	Transform& getTransform(void);

	virtual void update(float _dt, int _windowWidth, int _windowHeight) {}

	GameObject();
	GameObject(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale);
	
    bool isEnabled() const;

    bool isGameOver() const;

    bool isLevelWon() const;

    Mode mode = IDLE;

    bool destroyedForever = false;

	virtual ~GameObject();

	virtual void create();
	virtual void addComponent(Component* _component);
	virtual void destroy();
	virtual void addReceiver(GameObject *_go);
	virtual void readMessages();
	virtual void send(Message _m);
	void receive(Message _m);
	void sendComponents(Message _message);

    virtual void create(unsigned int _gameWidth);

    template<typename T>
	T* getComponent();

    int getAttackDamage();

    bool isInvincibleAfterHit();

    void addComponents(const std::vector<Component *> &_components);

    virtual void init();
    virtual void update(float _dt);
    void init(int _hp, int _attackDamage);
};

template <typename T>
T* GameObject::getComponent()
{
	for (Component* c : components)
	{
		T* t = dynamic_cast<T*>(c); //ugly but it works...
		if (t != nullptr)
		{
			return t;
		}
	}
	return nullptr;
}
