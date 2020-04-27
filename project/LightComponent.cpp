#include "LightComponent.h"
#include <glm/gtx/transform.hpp>
#include <iostream>

glm::vec3 LightComponent::getColor() const {
	return color;
}

float LightComponent::getInnerSpotlightAngle() const {
	return innerSpotlightAngle;
}

float LightComponent::getOuterSpotlightAngle() const {
	return outerSpotlightAngle;
}

float LightComponent::getIntensityMultiplier() const {
	return pointLightIntensityMultiplier;
}

LightComponent::LightComponent(const bool _movable, InputHandler* _inputHandler) {
	movable = _movable;
	inputHandler = _inputHandler;
}

glm::mat4 LightComponent::getProjMatrix() const {
	return glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
}

glm::mat4 LightComponent::getViewMatrix() const {
	return lookAt(go->getTransform().position, glm::vec3(0.0f), go->WORLD_UP);
}

void LightComponent::update(const float _dt) {
	if (inputHandler->getMouseStatus().isMouseRightDragging) {
		lightRotation += inputHandler->getMouseStatus().deltaX * ROTATION_SPEED * _dt;
		go->setPosition(glm::vec3(glm::rotate(lightRotation, go->WORLD_UP) * glm::vec4(go->getPosition(), 1.f)));
	}
}
