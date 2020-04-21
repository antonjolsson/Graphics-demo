#pragma once
#include "../../project/component.h"
#include <GL/glew.h>

#include "Model.h"
#include "Ship.h"

class RenderComponent : public Component
{
	labhelper::Model* model = nullptr;
	GLuint shaderProgram;
	glm::mat4 modelMatrix;
	
public:
	
	void update(float _dt) override;
	RenderComponent(GameObject* _go, GLuint _shaderProgram, labhelper::Model* _model);

};

