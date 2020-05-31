// ReSharper disable CppInconsistentNaming
#pragma once
#include <vector>



#include "AudioComponent.h"
#include "AudioComponent.h"
#include "AudioComponent.h"
#include "AudioComponent.h"
#include "AudioComponent.h"
#include "AudioComponent.h"
#include "AudioComponent.h"
#include "AudioComponent.h"
#include "InputHandler.h"
#include "ModelRenderComponent.h"
#include "CameraComponent.h"
#include "RenderComponent.h"
#include "ShadowMap.h"
#include "Ship.h"

class Renderer {

	
public:
	bool fog = false;
	float depthRange = 200;
	float fogDensity = 0.24;
	mat4 prevVPMatrix {1.f};
	
	bool depthOfField = false;
	float aperture = 0.3;
	int diaphragmPolygons = 10;

	bool ssao = true;

private:

	enum RenderPass {STANDARD, SHADOW, VIEW_NORMAL};
	std::map<int, RenderPass> renderPassMap {{1, VIEW_NORMAL}, {0, STANDARD}};
	
	int ssaoSamples = 16;
	float ssaoRadius = 1;
	GLuint ssaoInputProgram;
	GLuint randRotTex{};
	float randomRotations[64 * 64]{};
	Fbo depthNormalBuffer = Fbo(1);
	
	vec3 fogColor {1, 1, 1};
	
	const int AA_SAMPLES = 16;
	float environmentMultiplier = 2.5f;
	bool renderShadows = true;
	InputHandler* inputHandler;
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
	GameObject* background = nullptr;
	GameObject* landingPad = nullptr;

	std::vector<Fbo> fboList;

	GLuint dofProgram{};
	GLuint textureProgram;

public:
	void setShadowMapProgram(GLuint _shadowMapProgram);

	void createFrameBuffers(const int _winWidth, const int _winHeight);
	void genRandRotTex();
	Renderer(InputHandler* _inputHandler, GameObject* _camera, std::vector<RenderComponent*>* _renderComponents, bool _showHitbox,
	         int _winWidth, int _winHeight, std::vector<GameObject*>* _lights, Ship* _ship, GameObject* _background, GameObject* _landingPad);
	void setRenderShadows(bool _renderShadows);
	void setMatrixUniforms(GLuint _currentShaderProgram, const mat4& _viewMatrix, const mat4& _projectionMatrix, mat4 _modelMatrix);
	void drawScene(const ::Renderer::RenderPass _renderPass, mat4 _viewMatrix, mat4 _projMatrix, mat4 _lightViewMatrix, mat4 _lightProjMatrix);
	void drawShadowMap(mat4 _lightViewMatrix, mat4 _lightProjMatrix);
	void setLightUniforms(GLuint _currentShaderProgram, mat4 _viewMatrix, mat4 _lightViewMatrix, mat4 _lightProjMatrix,
	                      vec4 _viewSpaceLightPosition) const;
	void setFrameBuffer(GLuint _frameBufferId) const;
	void drawFromCamera(mat4 _projMatrix, mat4 _viewMatrix, mat4 _lightViewMatrix, mat4 _lightProjMatrix, RenderPass _renderPass);
	void prepareSSAO();
	void drawTextureToScreen(unsigned _texture) const;
	void draw();
	void drawBackground(const mat4& _viewMatrix, const mat4& _projectionMatrix);
	void drawWithDOF();

};
