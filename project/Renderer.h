// ReSharper disable CppInconsistentNaming
#pragma once
#include <vector>

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
	
	
	bool depthOfField = false;
	float aperture = 0.3;
	int diaphragmPolygons = 10;

	bool ssao = true;
	bool blurredSSAO = true;
	int ssaoSamples = 16;
	float ssaoRadius = 1.8;

	bool toneMapping = true;
	float gamma = 0.54;
	float exposure = 2.23;
	bool showOnlySSAO = false;
	float ssaoIntensity = 1.5;

	bool motionBlur = false;

private:
	
	GLuint currentProgram = 0;
	GLuint currentFboId = 0;
	std::array<GLuint, 16> boundTextures{};

	enum RenderPass {STANDARD, SHADOW, VIEW_NORMAL};
	std::map<int, RenderPass> renderPassMap {{1, VIEW_NORMAL}, {0, STANDARD}};
	
	GLuint ssaoInputProgram{};
	GLuint horizontalBlurProgram{};
	GLuint verticalBlurProgram{};
	GLuint randRotTex{};
	float randomRotations[64 * 64]{};
	Fbo viewNormalBuffer = Fbo(3);
	Fbo horizontalBlurBuffer, verticalBlurBuffer, ssaoNoiseBuffer, ssaoBlurBuffer;
	vec3 sampleHemisphere[16]{};
	
	vec3 fogColor {1, 1, 1};

	InputHandler* inputHandler;
	
	float environmentMultiplier = 2.5f;
	bool renderShadows = true;
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

	std::vector<Fbo> dofFboList;

	GLuint dofProgram{};
	GLuint textureProgram{};

	Fbo motionBlurBuffer;
	GLuint motionBlurProgram = 0;
	mat4 prevVPMatrix {1.f};
	mat4 prevProjMatrix {1.f};

	bool showTvTestScreen = true;

public:
	void setShadowMapProgram(GLuint _shadowMapProgram);

	void createFrameBuffers(const int _winWidth, const int _winHeight);
	void genRandomRotationsTex();
	void createPrograms();
	Renderer(InputHandler* _inputHandler, GameObject* _camera, std::vector<RenderComponent*>* _renderComponents, bool _showHitbox,
	         int _winWidth, int _winHeight, std::vector<GameObject*>* _lights, Ship* _ship, GameObject* _background, GameObject* _landingPad);
	void setRenderShadows(bool _renderShadows);
	void setMatrixUniforms(GLuint _currentShaderProgram, const mat4& _viewMatrix, const mat4& _projectionMatrix, mat4 _modelMatrix) const;
	void drawScene(const ::Renderer::RenderPass _renderPass, mat4 _viewMatrix, mat4 _projMatrix, mat4 _lightViewMatrix, mat4 _lightProjMatrix);
	void drawShadowMap(mat4 _lightViewMatrix, mat4 _lightProjMatrix);
	void setFXUniforms(GLuint _currentShaderProgram) const;
	void setLightUniforms(GLuint _currentShaderProgram, mat4 _viewMatrix, mat4 _lightViewMatrix, mat4 _lightProjMatrix,
	                      vec4 _viewSpaceLightPosition) const;
	void bindFrameBuffer(GLuint _frameBufferId);
	void setClearFrameBuffer(GLuint _frameBufferId);
	void drawFromCamera(mat4 _projMatrix, mat4 _viewMatrix, mat4 _lightViewMatrix, mat4 _lightProjMatrix, RenderPass _renderPass);
	void setRandomRotationsTex();
	void prepareSSAO();
	void drawTexture(GLuint _sourceTexture, GLuint _targetId, GLuint _program);
	void useProgram(GLuint _program);
	void createSSAOTexture();
	void bindTexture(GLenum _textureUnit, GLuint _texture);
	void applyMotionBlur(mat4 _viewMatrix, mat4 _projMatrix);
	void draw();
	void drawWithDOF();

};