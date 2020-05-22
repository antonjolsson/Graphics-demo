#include "Camera.h"

#include "CameraComponent.h"

Camera::Camera() {
	transform.position = DEFAULT_POSITION;
	enabled = true;
}

void Camera::update(const float _dt, const int _windowWidth, const int _windowHeight) {
    if (!isEnabled()) return;
    auto* cameraComponent = getComponent<CameraComponent>();
    cameraComponent->update(_dt, _windowHeight, _windowWidth);
    for (auto& component : components) {
        if (component != cameraComponent)
			component->update(_dt);
    }

	GameObject::update(_dt);
}
