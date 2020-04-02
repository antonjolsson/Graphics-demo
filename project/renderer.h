#pragma once
#include "../../project/component.h"
#include <GL/glew.h>

#include "Model.h"

class Renderer : public Component
{
	labhelper::Model* model = nullptr;
	GLuint shaderProgram;
	
public:

	void update(float _dt) override;
	Renderer(const GLuint _shaderProgram, labhelper::Model* _model);
};

