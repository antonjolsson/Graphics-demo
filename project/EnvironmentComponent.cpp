#include "EnvironmentComponent.h"

#include "labhelper.h"

std::string EnvironmentComponent::getEnvmapBaseName() const {
	return ENVMAP_BASE_NAME;
}

float EnvironmentComponent::getEnvironmentMultiplier() const {
	return environmentMultiplier;
}

GLuint EnvironmentComponent::getEnvironmentMap() const {
	return environmentMap;
}

void EnvironmentComponent::setEnvironmentMap(GLuint _environmentMap) {
	environmentMap = _environmentMap;
}

void EnvironmentComponent::setIrradianceMap(GLuint _irradianceMap) {
	irradianceMap = _irradianceMap;
}

void EnvironmentComponent::setReflectionMap(GLuint _reflectionMap) {
	reflectionMap = _reflectionMap;
}

GLuint EnvironmentComponent::getIrradianceMap() const {
	return irradianceMap;
}

GLuint EnvironmentComponent::getReflectionMap() const {
	return reflectionMap;
}

void EnvironmentComponent::bindEnvMaps() const {
	if (environmentMap) {
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, environmentMap);
	}
	if (irradianceMap) {
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, irradianceMap);
	}
	if (reflectionMap) {
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, reflectionMap);
	}
	glActiveTexture(GL_TEXTURE0);
}

void EnvironmentComponent::draw(glm::mat4 _viewMatrix, glm::mat4 _projMatrix, GameObject* _camera) const {
	glUseProgram(environmentProgram);
	labhelper::setUniformSlow(environmentProgram, "environment_multiplier", environmentMultiplier);
	labhelper::setUniformSlow(environmentProgram, "inv_PV", glm::inverse(_projMatrix * _viewMatrix));
	labhelper::setUniformSlow(environmentProgram, "camera_pos", _camera->getTransform().position);
	labhelper::drawFullScreenQuad();
}

EnvironmentComponent::EnvironmentComponent(GameObject* _background, const GLuint _environmentProgram) {
	go = _background;
	environmentProgram = _environmentProgram;
}
