#pragma once
#include <glm/glm.hpp>

#include "component.h"

class LightComponent : public Component {

	glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);
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

	LightComponent();

	glm::mat4 getProjMatrix();
	glm::mat4 getViewMatrix();
};

