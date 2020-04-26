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

vec3 CameraComponent::getCameraDirection() const {
	return cameraDirection;
}

void CameraComponent::setCameraDirection(const vec3& _cameraDirection) {
	cameraDirection = _cameraDirection;
}

void CameraComponent::init(GameObject* _camera) {
	addGameObject(_camera);
	if (tracingObject) traceObject();
	else setCameraDirection(normalize(vec3(0.0f) - _camera->getTransform().position));
}

void CameraComponent::setTracingObject(const bool _tracingObject) {
	tracingObject = _tracingObject;
}

CameraComponent::CameraComponent(GameObject* _tracing, const int _winWidth, const int _winHeight, 
	InputHandler* _inputHandler) {
	tracingObject = true;
	tracing = _tracing;
	windowWidth = _winWidth;
	windowHeight = _winHeight;
	inputHandler = _inputHandler;
}

void CameraComponent::traceObject() {
	go->getTransform().position = tracing->getTransform().position + tracingDistance;
	cameraDirection = normalize(tracing->getTransform().position - go->getTransform().position);
}

auto CameraComponent::update(float _dt, const int _windowHeight, const int _windowWidth) -> void {
	windowWidth = _windowWidth;
	windowHeight = _windowHeight;
	if (tracingObject) {
		traceObject();
	}
	else {
		keyStatus = inputHandler->getKeyStatus();
		if (keyStatus.lowerCamera) go->getTransform().position -= _dt * speed * go->WORLD_UP;
		if (keyStatus.raiseCamera) go->getTransform().position += _dt * speed * go->WORLD_UP;
		if (keyStatus.forwardCamera) go->getTransform().position += _dt * speed * cameraDirection;
		if (keyStatus.backwardCamera) go->getTransform().position -= _dt * speed * cameraDirection;
	}
}

mat4 CameraComponent::getProjMatrix() const {
	return perspective(radians(fieldOfView), float(windowWidth) / float(windowHeight), nearPlane, farPlane);
}

mat4 CameraComponent::getViewMatrix() const {
	return lookAt(go->getTransform().position, go->getTransform().position + cameraDirection, go->WORLD_UP);
}
