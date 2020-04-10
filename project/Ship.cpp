#include "rigidBody.h"
#include "Ship.h"

#include "behaviour.h"

Ship::Ship(Engine* _engine, GLuint _shaderProgram, const bool _showHitbox) {
	transform.position = INITIAL_POSITION;
	auto* behaviour = new Behaviour(this, _engine);
	auto* renderer = new Renderer(_shaderProgram, fighterModel);
	auto* rigidBody = new RigidBody(this, DRAG_COEFF, X_ACCELERATION, MAX_SHIP_X_ROT, MAX_SHIP_X_ROTATION_SPEED,
		MAX_SHIP_Y_ROTATION_SPEED, CLAMP_ROT_TO_ZERO_SPEED);
	GameObject::addComponents(std::vector<Component*> {behaviour, renderer, rigidBody});
}

void Ship::update(const float _dt)
{
	GameObject::update(_dt);
}
