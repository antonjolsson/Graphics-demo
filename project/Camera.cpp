#include "Camera.h"

#include "CameraComponent.h"

Camera::Camera() {
	
	transform.position = DEFAULT_POSITION;
}

void Camera::update(const float _dt, const int _windowWidth, const int _windowHeight) {
	getComponent<CameraComponent>()->update(_dt, _windowWidth, _windowHeight);
}
