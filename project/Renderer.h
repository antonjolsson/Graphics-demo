#pragma once
#include <vector>
#include "engine.h"
#include "RenderComponent.h"
#include "CameraComponent.h"

class Renderer {
	
	const int AA_SAMPLES = 16;
	bool renderShadows = false;
	Engine* engine;
	GameObject* camera;
	const std::vector<RenderComponent*>& renderComponents;
	bool showHitbox = false;
	int winWidth;
	int winHeight;
	CameraComponent* cameraComponent;

public:
	
	Renderer(Engine* _engine, GameObject* _camera, const std::vector<RenderComponent*>& _renderComponents, 
		bool _showHitbox, int _winWidth, int _winHeight);
	void setRenderShadows(bool _renderShadows);
	void draw();

};
