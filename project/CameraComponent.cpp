#include "CameraComponent.h"
#include <glm/gtx/transform.hpp>

void CameraComponent::setFieldOfView(const float _fieldOfView) {
	fieldOfView = _fieldOfView;
}

float CameraComponent::getNearPlane() const {
	return nearPlane;
}

float CameraComponent::getFarPlane() const {
	return farPlane;
}

void CameraComponent::setCameraDirection(const vec3& _cameraDirection) {
	cameraDirection = _cameraDirection;
}

void CameraComponent::init(GameObject* _camera) {
	addGameObject(_camera);
	if (tracingObject) traceObject();
	setCameraDirection(normalize(vec3(0.0f) - _camera->getTransform().position));
}

CameraComponent::CameraComponent(GameObject* _tracing, const int _winWidth, const int _winHeight) {
	tracingObject = true;
	tracing = _tracing;
	windowWidth = _winWidth;
	windowHeight = _winHeight;
}

void CameraComponent::traceObject() {
	go->getTransform().position = tracing->getTransform().position += tracingDistance;
	cameraDirection = normalize(tracing->getTransform().position - go->getTransform().position);
}

void CameraComponent::update(float _dt, const int _windowHeight, const int _windowWidth) {
	windowWidth = _windowWidth;
	windowHeight = _windowHeight;
	if (tracingObject) {
		traceObject();
	}
}

mat4 CameraComponent::getProjMatrix() const {
	return perspective(radians(fieldOfView), float(windowWidth) / float(windowHeight), nearPlane, farPlane);
}

mat4 CameraComponent::getViewMatrix() const {
	return lookAt(go->getTransform().position, go->getTransform().position + cameraDirection, go->WORLD_UP);
}
