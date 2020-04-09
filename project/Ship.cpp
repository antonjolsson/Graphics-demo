#include "rigidBody.h"
#include "ship.h"

ship::Ship::Ship(const GLuint _shaderProgram)
{
	transform.position = INITIAL_POSITION;
	auto* renderer = new Renderer(_shaderProgram, fighterModel);
	auto* rigidBody = new RigidBody(this, DRAG_COEFF, X_ACCELERATION, MAX_SHIP_X_ROT, MAX_SHIP_X_ROTATION_SPEED,
	                                MAX_SHIP_Y_ROTATION_SPEED, CLAMP_ROT_TO_ZERO_SPEED);
	components.push_back(renderer);
}

void ship::Ship::update(float _dt)
{
	GameObject::update(_dt);
}
