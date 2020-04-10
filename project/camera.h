#pragma once

#include "gameObject.h"

class Camera :public GameObject
{
	
	Engine* engine;
	
public:
	
	Camera(Engine* _engine);
	
};

