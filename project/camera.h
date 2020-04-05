#pragma once

#include "gameObject.h"

class Camera :public GameObject
{

public:
	
	Camera(AvancezLib* _engine);
	AvancezLib* engine;
};

