#include "ship.h"

Ship::Ship(const GLuint _shaderProgram)
{
	auto* renderer = new Renderer(_shaderProgram, fighterModel);
	components.push_back(renderer);
}

void Ship::update(float _dt)
{
	
}
