#pragma once
#include <vector>

#include "engine.h"
#include "RenderComponent.h"
#include "CameraComponent.h"
#include "ShadowMap.h"

class Renderer {
	
	const int AA_SAMPLES = 16;
	float environmentMultiplier = 2.5f;
	bool renderShadows = false;
	Engine* engine;
	GameObject* camera;
	const std::vector<RenderComponent*>* renderComponents;
	bool showHitbox = false;
	int winWidth;
	int winHeight;
	CameraComponent* cameraComponent = nullptr;
	std::vector<GameObject*>* lights = nullptr;
	Ship* ship; // todo: remove
	
	ShadowMap* shadowMap = nullptr;
	GLuint shadowMapProgram = 0;

public:
	void setShadowMapProgram(GLuint _shadowMapProgram);

	Renderer(Engine* _engine, GameObject* _camera, std::vector<RenderComponent*>* _renderComponents, bool _showHitbox,
	         int _winWidth, int _winHeight, std::vector<GameObject*>* _lights, Ship* _ship);
	void setRenderShadows(bool _renderShadows);
	void drawScene(GLuint _shaderProgram, mat4 _viewMatrix, mat4 _projMatrix, mat4 _lightViewMatrix, mat4 _lightProjMatrix) const;
	void drawShadowMap(mat4 _viewMatrix, mat4 _projMatrix, mat4 _lightViewMatrix, mat4 _lightProjMatrix);
	void bindEnvironmentMaps();
	void setLightUniforms(const GLuint _currentShaderProgram, mat4 _viewMatrix, mat4 _lightViewMatrix, mat4 _lightProjMatrix, 
		const vec4 _viewSpaceLightPosition) const;
	void drawFromCamera(mat4 _projMatrix, mat4 _viewMatrix, mat4 _lightViewMatrix, mat4 _lightProjMatrix);
	void draw();

};
