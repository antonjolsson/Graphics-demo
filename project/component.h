#pragma once

#include <set>
#include <array>

#include "engine.h"
#include "objectPool.h"
#include "gameObject.h"

class GameObject;

class Component {

protected:

    //const Vector2D GRAVITY {0, 600};
    Engine* engine = nullptr;	// used to access the engine
	GameObject* go = nullptr;		// the game object this component is part of
	std::set<GameObject*>* gameObjects = nullptr;	// the global container of game objects

    bool paused = false;

public:

    virtual ~Component() = default;

	virtual void init() {}
	virtual void update(float _dt) {};
	virtual void receive(int _message);
	virtual void destroy() {}
};

