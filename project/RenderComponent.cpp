#include "RenderComponent.h"


#include <glm/gtx/euler_angles.inl>
#include <glm/gtx/transform.hpp>

void RenderComponent::update(float _dt)
{
	const glm::mat4 rotMatrix = glm::eulerAngleYXZ(go->getTransform().position.y, go->getTransform().position.x,
		0.f); // TODO: make generic?
	const glm::vec4 translation = modelMatrix[3];
	modelMatrix = rotMatrix * glm::mat4(1.f);
	modelMatrix[3] = translation;

	//modelMatrix = glm::translate(modelMatrix, shipSpeed * -xAxis);
}

RenderComponent::RenderComponent(GameObject* _go, GLuint _shaderProgram, labhelper::Model* _model): modelMatrix() {
	go = _go;
	shaderProgram = _shaderProgram;
	model = _model;
}
