
#include <cmath>
#include <set>
#include "avancezlib.h"
#include "gameObject.h"
#include "component.h"

CompType Component::getCompType() const
{
	return compType;
}

void Component::create(AvancezLib* _engine, GameObject* _go, std::set<GameObject*>* _gameObjects)
{
	go = _go;
	engine = _engine;
	gameObjects = _gameObjects;
}

void Component::receive(int message) {
    switch (message) { // NOLINT(hicpp-multiway-paths-covered)
    case GAME_OVER: case MSG_EXPLODED:
            paused = true;
            break;
        default:
            break;
    }
}

