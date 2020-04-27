#pragma once
#include <glm/glm.hpp>

#include "component.h"

class LightComponent : public Component {
	const float ROTATION_SPEED = 0.01f;
	glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);
	bool movable = false;
	InputHandler* inputHandler = nullptr;
	float lightRotation = 0;
public:
	glm::vec3 getColor() const;
	float getInnerSpotlightAngle() const;
	float getOuterSpotlightAngle() const;
	float getIntensityMultiplier() const;
private:
	bool useSpotLight = true;
	float innerSpotlightAngle = 20.3f;
	float outerSpotlightAngle = 24.0f;
	float pointLightIntensityMultiplier = 10000.0f;

	float fieldOfView = glm::radians(45.f);
	float aspectRatio = 1;
	float nearPlane = 25;
	float farPlane = 100;
	
public:

	LightComponent(bool _movable, InputHandler* _inputHandler);

	glm::mat4 getProjMatrix() const;
	glm::mat4 getViewMatrix() const;

	void update(float _dt) override;

};

