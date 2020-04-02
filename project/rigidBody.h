#pragma once
#include "../../project/component.h"

class RigidBody : public Component
{
	glm::vec3 acceleration{ 0 };
	glm::vec3 velocity{ 0 };
	
	void update(float dt) override;
};

