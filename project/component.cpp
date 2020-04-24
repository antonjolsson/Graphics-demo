
#include <cmath>
#include <set>
#include "gameObject.h"
#include "component.h"

void Component::receive(int _message) {
    switch (_message) { // NOLINT(hicpp-multiway-paths-covered)
    case GAME_OVER: case MSG_EXPLODED:
            paused = true;
            break;
        default:
            break;
    }
}

void Component::addGameObject(GameObject* _go) {
    go = _go;
}

