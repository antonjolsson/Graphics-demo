#pragma once
#include "../../project/gameObject.h"
#include "renderer.h"
#include <GL/glew.h>

class Ship : public GameObject
{
	labhelper::Model* fighterModel = labhelper::loadModelFromOBJ("../scenes/NewShip.obj");

public:
	
	explicit Ship(GLuint _shaderProgram);

	void update(float _dt) override;
};

