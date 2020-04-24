#include "CameraComponent.h"
#include <glm/gtx/transform.hpp>

void CameraComponent::setCameraDirection(const vec3& _cameraDirection) {
	cameraDirection = _cameraDirection;
}

CameraComponent::CameraComponent(Ship* _tracing) : tracing(_tracing), windowWidth(0), windowHeight(0) {
	tracingObject = true;
}

void CameraComponent::update(float _dt, const int _windowHeight, const int _windowWidth) {
	windowWidth = _windowWidth;
	windowHeight = _windowHeight;
	if (tracingObject) {
		go->getTransform().position = tracing->getTransform().position += tracingDistance;
		cameraDirection = normalize(vec3(0.0f) - go->getTransform().position);
	}
}

mat4 CameraComponent::getProjMatrix() const {
	return perspective(radians(fieldOfView), float(windowWidth) / float(windowHeight), nearPlane, farPlane);
}

mat4 CameraComponent::getViewMatrix() const {
	return lookAt(go->getTransform().position, go->getTransform().position + cameraDirection, worldUp);
}
