#include "LightComponent.h"
#include <glm/gtx/transform.hpp>

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

LightComponent::LightComponent() {
	
}

glm::mat4 LightComponent::getProjMatrix() {
	return glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
}

glm::mat4 LightComponent::getViewMatrix() {
	return lookAt(go->getTransform().position, glm::vec3(0.0f), go->WORLD_UP);
}
