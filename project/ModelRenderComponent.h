#pragma once
#include "../../project/component.h"
#include <GL/glew.h>

#include "Model.h"

class ModelRenderComponent : public Component
{
	labhelper::Model* model = nullptr;
	GLuint shaderProgram;
public:
	GLuint getShaderProgram() const;
private:
	glm::mat4 modelMatrix;

public:
	labhelper::Model* getModel() const;
	glm::mat4 getModelMatrix() const;

	void update(float _dt) override;
	ModelRenderComponent(GameObject* _go, const GLuint _shaderProgram, labhelper::Model* _model, glm::mat4 _modelMatrix);

};

