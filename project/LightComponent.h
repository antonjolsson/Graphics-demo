#pragma once
#include "component.h"

class LightComponent : public Component {

	glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);
	bool useSpotLight = true;
	float innerSpotlightAngle = 20.3f;
	float outerSpotlightAngle = 24.0f;
	float pointLightIntensityMultiplier = 10000.0f;
	
public:

	LightComponent();

};

