#pragma once
#include <vector>

#include "InputHandler.h"
#include "ModelRenderComponent.h"
#include "CameraComponent.h"
#include "ShadowMap.h"
#include "Ship.h"

class Renderer {
	
	const int AA_SAMPLES = 16;
	float environmentMultiplier = 2.5f;
	bool renderShadows = false;
	InputHandler* engine;
	GameObject* camera;
	const std::vector<ModelRenderComponent*>* renderComponents;
	bool showHitbox = false;
	int winWidth;
	int winHeight;
	CameraComponent* cameraComponent = nullptr;
	std::vector<GameObject*>* lights = nullptr;
	Ship* ship; // todo: remove
	
	ShadowMap* shadowMap = nullptr;
	GLuint shadowMapProgram = 0;
	GameObject* background = nullptr;

public:
	void setShadowMapProgram(GLuint _shadowMapProgram);

	Renderer(InputHandler* _engine, GameObject* _camera, std::vector<ModelRenderComponent*>* _renderComponents, bool _showHitbox,
	         int _winWidth, int _winHeight, std::vector<GameObject*>* _lights, Ship* _ship, GameObject* _background);
	void setRenderShadows(bool _renderShadows);
	void drawScene(GLuint _shaderProgram, mat4 _viewMatrix, mat4 _projMatrix, mat4 _lightViewMatrix, mat4 _lightProjMatrix) const;
	void drawShadowMap(mat4 _viewMatrix, mat4 _projMatrix, mat4 _lightViewMatrix, mat4 _lightProjMatrix) const;
	void bindEnvironmentMaps();
	void setLightUniforms(const GLuint _currentShaderProgram, mat4 _viewMatrix, mat4 _lightViewMatrix, mat4 _lightProjMatrix, 
		const vec4 _viewSpaceLightPosition) const;
	void drawFromCamera(mat4 _projMatrix, mat4 _viewMatrix, mat4 _lightViewMatrix, mat4 _lightProjMatrix) const;
	void drawBackground(const mat4& _viewMatrix, const mat4& _projectionMatrix);
	void draw() const;

};
