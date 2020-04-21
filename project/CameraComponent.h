#pragma once

#include "gameObject.h"

class CameraComponent :public GameObject
{
	
	Engine* engine;
	
public:
	
	CameraComponent(Engine* _engine);
	
};

