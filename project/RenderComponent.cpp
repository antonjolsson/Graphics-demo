#include "RenderComponent.h"

#include "labhelper.h"

GLuint RenderComponent::getDefaultProgram() const {
	return defaultProgram;
}

glm::mat4 RenderComponent::getModelMatrix() const {
	return modelMatrix;
}

GLuint RenderComponent::getViewNormalProgram() const {
	return viewNormalProgram;
}

RenderComponent::RenderComponent() {
	// viewNormalProgram = labhelper::loadShaderProgram("../project/shading.vert", "../project/viewNormal.frag");
}
