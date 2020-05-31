#include "ModelRenderComponent.h"

#include <glm/gtx/euler_angles.inl>
#include <glm/gtx/transform.hpp>

#include "labhelper.h"

void ModelRenderComponent::render(const GLuint _compShaderProgram) {
	labhelper::render(model);
}

labhelper::Model* ModelRenderComponent::getModel() const {
	return model;
}

void ModelRenderComponent::update(float _dt)
{
	modelMatrix = go->getModelMatrix();
}

ModelRenderComponent::ModelRenderComponent(GameObject* _go, const GLuint _shaderProgram, labhelper::Model* _model,
                                           const glm::mat4 _modelMatrix) {
	go = _go;
	defaultProgram = _shaderProgram;
	viewNormalProgram = labhelper::loadShaderProgram("../project/shading.vert", "../project/viewNormal.frag");
	model = _model;
	modelMatrix = _modelMatrix;
}