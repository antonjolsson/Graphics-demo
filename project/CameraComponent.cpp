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

void CameraComponent::setMouseMovable(bool _mouseMovable) {
	mouseMovable = _mouseMovable;
}

glm::vec3 CameraComponent::getCameraDirection() const {
	return cameraDirection;
}

void CameraComponent::setCameraDirection(const glm::vec3& _cameraDirection) {
	cameraDirection = _cameraDirection;
}

void CameraComponent::init(GameObject* _camera) {
	addGameObject(_camera);
	if (tracingObject) traceObject();
	else setCameraDirection(glm::normalize(glm::vec3(0.0f) - _camera->getTransform().position));
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

void CameraComponent::moveCamera(const float _dt) {
	keyStatus = inputHandler->getKeyStatus();
	if (keyStatus.lowerCamera) go->getTransform().position -= _dt * speed * go->Y_AXIS;
	if (keyStatus.raiseCamera) go->getTransform().position += _dt * speed * go->Y_AXIS;
	if (keyStatus.forwardCamera) go->getTransform().position += _dt * speed * cameraDirection;
	if (keyStatus.backwardCamera) go->getTransform().position -= _dt * speed * cameraDirection;

	if (mouseMovable) {
		mouse = inputHandler->getMouseStatus();
		if (mouse.isDragging) {
			const glm::mat4 yaw = rotate(ROTATION_SPEED * -mouse.deltaX, go->Y_AXIS);
			const glm::mat4 pitch = rotate(ROTATION_SPEED * -mouse.deltaY, normalize(cross(cameraDirection, go->Y_AXIS)));
			cameraDirection = glm::vec3(pitch * yaw * glm::vec4(cameraDirection, 0.0f));
		}
	}
}

auto CameraComponent::update(const float _dt, const int _windowHeight, const int _windowWidth) -> void {
	windowWidth = _windowWidth;
	windowHeight = _windowHeight;
	if (tracingObject) {
		traceObject();
	}
	else moveCamera(_dt);
}

glm::mat4 CameraComponent::getProjMatrix() const {
	return glm::perspective(glm::radians(fieldOfView), float(windowWidth) / float(windowHeight), nearPlane, farPlane);
}

glm::mat4 CameraComponent::getViewMatrix() const {
	return lookAt(go->getTransform().position, go->getTransform().position + cameraDirection, go->Y_AXIS);
}
