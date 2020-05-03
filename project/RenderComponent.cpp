#include "RenderComponent.h"

GLuint RenderComponent::getShaderProgram() const {
	return shaderProgram;
}

glm::mat4 RenderComponent::getModelMatrix() const {
	return modelMatrix;
}
