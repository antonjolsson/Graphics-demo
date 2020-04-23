#pragma once
#include <vector>
#include "engine.h"
#include "RenderComponent.h"
#include "CameraComponent.h"

class Renderer {
	
	
	bool renderShadows = false;
	Engine* engine;
	CameraComponent* camera;
	const std::vector<RenderComponent*>& renderComponents;
	bool showHitbox = false;

public:
	
	Renderer(Engine* _engine, CameraComponent* _camera, const std::vector<RenderComponent*>& _renderComponents, bool _showHitbox);
	void setRenderShadows(bool _renderShadows);
	void draw();

};
