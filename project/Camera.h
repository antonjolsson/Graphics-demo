#pragma once

#include "gameObject.h"

class Camera : public GameObject {

public:
	Camera();
private:
	const glm::vec3 DEFAULT_POSITION {50, 50, 0};

public:

	void update(float _dt, int _windowWidth, int _windowHeight) override;
	int windowWidth;
	int windowHeight;
};
