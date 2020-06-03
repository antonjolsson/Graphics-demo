// ReSharper disable IdentifierTypo
#include "Renderer.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EnvironmentComponent.h"
#include "LightComponent.h"
#include "RenderComponent.h"
#include "Ship.h"

void Renderer::setShadowMapProgram(const GLuint _shadowMapProgram) {
	shadowMapProgram = _shadowMapProgram;
}

void Renderer::createFrameBuffers(const int _winWidth, const int _winHeight) {
	const int numFbos = 10;
	for (int i = 0; i < numFbos; i++) {
		fboList.emplace_back(Fbo(1));
		fboList[i].resize(_winWidth, _winHeight);
	}
	viewNormalBuffer.resize(_winWidth, _winHeight);
	ssaoTexture.resize(_winWidth, _winHeight);
}

void Renderer::genRandRotTex() {
	glGenTextures(1, &randRotTex);
	glBindTexture(GL_TEXTURE_2D,randRotTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Renderer::Renderer(InputHandler* _inputHandler, GameObject* _camera, std::vector<RenderComponent*>* _renderComponents, 
                   const bool _showHitbox, const int _winWidth, const int _winHeight, std::vector<GameObject*>* _lights,
                   Ship* _ship, GameObject* _background, GameObject* _landingPad) :
	inputHandler(_inputHandler), camera(_camera), renderComponents(_renderComponents), showHitbox(_showHitbox), winWidth(_winWidth),
	winHeight(_winHeight), lights(_lights) {
	ship = _ship;
	landingPad = _landingPad;
	background = _background;
	cameraComponent = camera->getComponent<CameraComponent>();
	glEnable(GL_DEPTH_TEST); // enable Z-buffering
	glEnable(GL_CULL_FACE);  // enables backface culling
	//glActiveTexture(GL_TEXTURE0);

	createFrameBuffers(winWidth, winHeight);
	dofProgram = labhelper::loadShaderProgram("../project/postFX.vert", "../project/dof.frag");
	ssaoInputProgram = labhelper::loadShaderProgram("../project/ssaoInput.vert", 
		"../project/ssaoInput.frag");
	textureProgram = labhelper::loadShaderProgram("../project/texture.vert", 
		"../project/texture.frag");
	
	prepareSSAO();
}

void Renderer::setRenderShadows(const bool _renderShadows) {
	renderShadows = _renderShadows;
	if (renderShadows) {
		shadowMap = new ShadowMap(shadowMapProgram);
	}
}

void Renderer::setMatrixUniforms(const GLuint _currentShaderProgram, const mat4& _viewMatrix, const mat4& _projectionMatrix, 
                       const mat4 _modelMatrix) {
	labhelper::setUniformSlow(_currentShaderProgram, "prevVPMatrix", prevVPMatrix);
	labhelper::setUniformSlow(_currentShaderProgram, "modelViewProjectionMatrix",
		_projectionMatrix * _viewMatrix * _modelMatrix);
	prevVPMatrix = _projectionMatrix * _viewMatrix;
	labhelper::setUniformSlow(_currentShaderProgram, "modelViewMatrix", _viewMatrix * _modelMatrix);
	mat4 normal = inverse(transpose(_viewMatrix * _modelMatrix));
	labhelper::setUniformSlow(_currentShaderProgram, "normalMatrix",
		inverse(transpose(_viewMatrix * _modelMatrix)));
	labhelper::setUniformSlow(_currentShaderProgram, "modelMatrix", _modelMatrix);
}

void Renderer::drawScene(const RenderPass _renderPass, const mat4 _viewMatrix, const mat4 _projMatrix, const mat4 _lightViewMatrix,
                         const mat4 _lightProjMatrix){
	const vec4 viewSpaceLightPosition = _viewMatrix * vec4((*lights)[0]->getTransform().position, 1.0f);
	for (auto renderComponent : *renderComponents) {
		GLuint compShaderProgram;
		switch (_renderPass) {
		case SHADOW:
			compShaderProgram = shadowMapProgram;
			break;
		case VIEW_NORMAL:
			compShaderProgram = renderComponent->getViewNormalProgram();
			break;
		case STANDARD: default: 
			compShaderProgram = renderComponent->getDefaultProgram();
		}
		glUseProgram(compShaderProgram);
		setLightUniforms(compShaderProgram, _viewMatrix, _lightViewMatrix, _lightProjMatrix,
			viewSpaceLightPosition);
		setMatrixUniforms(compShaderProgram, _viewMatrix, _projMatrix, renderComponent->getModelMatrix());

		bindTexture(GL_TEXTURE10, shadowMap->getShadowMapFB().depthBuffer);

		renderComponent->render(compShaderProgram);
	}
}

void Renderer::drawShadowMap(const mat4 _lightViewMatrix, const mat4 _lightProjMatrix) {
	shadowMap->draw();
	drawScene(SHADOW, _lightViewMatrix, _lightProjMatrix, _lightViewMatrix,
	          _lightProjMatrix);
	if (landingPad != nullptr) {
		labhelper::Material& screen = landingPad->getComponent<ModelRenderComponent>()->getModel()->m_materials[8];
		screen.m_emission_texture.gl_id = shadowMap->getShadowMapFB().colorTextureTargets[0];
	}

	if (shadowMap->usesPolygonOffset())
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}

void Renderer::setLightUniforms(const GLuint _currentShaderProgram, mat4 _viewMatrix, const mat4 _lightViewMatrix,
                                const mat4 _lightProjMatrix, const vec4 _viewSpaceLightPosition) const {
	const auto lightComp = (*lights)[0]->getComponent<LightComponent>(); // todo: refactor for several lights
	labhelper::setUniformSlow(_currentShaderProgram, "point_light_color", lightComp->getColor());
	labhelper::setUniformSlow(_currentShaderProgram, "point_light_intensity_multiplier",
		lightComp->getIntensityMultiplier());
	labhelper::setUniformSlow(_currentShaderProgram, "viewSpaceLightPosition", vec3(_viewSpaceLightPosition));
	labhelper::setUniformSlow(_currentShaderProgram, "viewSpaceLightDir",
		normalize(vec3(_viewMatrix * vec4(vec3(ship->getComponent<ModelRenderComponent>()->getModelMatrix()[3]) - 
			(*lights)[0]->getTransform().position, 0.0f)))); // TODO: remove ship dependency
	labhelper::setUniformSlow(_currentShaderProgram, "spotOuterAngle",
		std::cos(radians(lightComp->getOuterSpotlightAngle())));
	labhelper::setUniformSlow(_currentShaderProgram, "spotInnerAngle",
		std::cos(radians(lightComp->getInnerSpotlightAngle())));

	const mat4 lightMatrix = translate(vec3(0.5f)) * scale(vec3(0.5f)) * _lightProjMatrix * 
		_lightViewMatrix * inverse(_viewMatrix);
	labhelper::setUniformSlow(_currentShaderProgram, "lightMatrix", lightMatrix);

	// Environment
	labhelper::setUniformSlow(_currentShaderProgram, "environment_multiplier", environmentMultiplier);

	// camera
	labhelper::setUniformSlow(_currentShaderProgram, "viewInverse", inverse(_viewMatrix));

	labhelper::setUniformSlow(_currentShaderProgram, "fog", fog);
	labhelper::setUniformSlow(_currentShaderProgram, "fogColor", fogColor);
	labhelper::setUniformSlow(_currentShaderProgram, "fogDensity", fogDensity);
	labhelper::setUniformSlow(_currentShaderProgram, "depthRange", depthRange);
	
	labhelper::setUniformSlow(_currentShaderProgram, "toneMapping", toneMapping);
	labhelper::setUniformSlow(_currentShaderProgram, "gamma", gamma);
	labhelper::setUniformSlow(_currentShaderProgram, "exposure", exposure);

	labhelper::setUniformSlow(_currentShaderProgram, "ssao", ssao);
}

void Renderer::setClearFrameBuffer(const GLuint _frameBufferId) const {
	glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferId);
	glViewport(0, 0, winWidth, winHeight);
	glClearColor(0.2f, 0.2f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::drawFromCamera(const mat4 _projMatrix, const mat4 _viewMatrix, const mat4 _lightViewMatrix,
                              const mat4 _lightProjMatrix, const RenderPass _renderPass){
	if (!depthOfField) {
		if (_renderPass == STANDARD)
			setClearFrameBuffer(0);
		else setClearFrameBuffer(viewNormalBuffer.framebufferId);
	}
		
	if (background != nullptr && _renderPass == STANDARD) {
		const vec4 viewSpaceLightPosition = _viewMatrix * vec4((*lights)[0]->getTransform().position, 1.0f);
		glUseProgram(background->getComponent<EnvironmentComponent>()->environmentProgram);
		setLightUniforms(background->getComponent<EnvironmentComponent>()->environmentProgram, _viewMatrix, 
			_lightViewMatrix, _lightProjMatrix, viewSpaceLightPosition);
		background->getComponent<EnvironmentComponent>()->draw(_viewMatrix, _projMatrix, camera);
	}
	drawScene(_renderPass, _viewMatrix, _projMatrix, _lightViewMatrix,
	          _lightProjMatrix);
}

void Renderer::setRandRotTex() {
	for (float& randomRotation : randomRotations) {
		randomRotation = labhelper::randf();
	}

	bindTexture(GL_TEXTURE0, randRotTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, 64, 64, 0, GL_RED, GL_FLOAT, 
	             randomRotations);
}

void Renderer::prepareSSAO() {
	for (int i = 0; i < ssaoSamples; i++) {
		vec3 sample = labhelper::cosineSampleHemisphere();
		sample *= labhelper::randf();
		sampleHemisphere[i] = sample;
	}

	genRandRotTex();
	setRandRotTex();
}

void Renderer::drawSSAOTexture() {
	glUseProgram(ssaoInputProgram);

	bindTexture(GL_TEXTURE1, randRotTex);
	bindTexture(GL_TEXTURE2, viewNormalBuffer.depthBuffer);
	bindTexture(GL_TEXTURE3, viewNormalBuffer.colorTextureTargets[2]);

	labhelper::setUniformSlow(ssaoInputProgram, "inverseProjMatrix",
	                          inverse(cameraComponent->getProjMatrix()));
	labhelper::setUniformSlow(ssaoInputProgram, "projectionMatrix", cameraComponent->getProjMatrix());
	labhelper::setUniformSlow(ssaoInputProgram, "samples", ssaoSamples, sampleHemisphere);
	labhelper::setUniformSlow(ssaoInputProgram, "hemisphereRadius", ssaoRadius);
	labhelper::setUniformSlow(ssaoInputProgram, "numOfSamples", ssaoSamples);
	
	drawTexture(viewNormalBuffer.colorTextureTargets[0], ssaoTexture.framebufferId, ssaoInputProgram);
}

void Renderer::bindTexture(const GLenum _textureUnit, const GLuint _texture) {
	glActiveTexture(_textureUnit);
	glBindTexture(GL_TEXTURE_2D, _texture);
	boundTextures[_textureUnit - GL_TEXTURE0] = _texture; 
}

void Renderer::drawTexture(const GLuint _sourceTexture, const GLuint _targetId, const GLuint _program) {
	setClearFrameBuffer(_targetId);
	glUseProgram(_program);

	bindTexture(GL_TEXTURE0, _sourceTexture);

	labhelper::drawFullScreenQuad();

	glUseProgram(0);
}

void Renderer::draw(){
	if (depthOfField) {
		drawWithDOF();
		return;
	}

	int iteration = 0;
	if (ssao) {
		//setRandRotTex();
		iteration = 1;
	}
	
	for (; iteration >= 0; --iteration) {
		const RenderPass renderPass = renderPassMap[iteration];
		
		const mat4 projMatrix = cameraComponent->getProjMatrix();
		const mat4 viewMatrix = cameraComponent->getViewMatrix();
		const mat4 lightProjMatrix = (*lights)[0]->getComponent<LightComponent>()->getProjMatrix();
		const mat4 lightViewMatrix = (*lights)[0]->getComponent<LightComponent>()->getViewMatrix();

		if (background != nullptr && renderPass == STANDARD) 
			background->getComponent<EnvironmentComponent>()->bindEnvMaps();

		if (renderShadows && renderPass == STANDARD) 
			drawShadowMap(lightViewMatrix, lightProjMatrix);

		drawFromCamera(projMatrix, viewMatrix, lightViewMatrix,
		               lightProjMatrix, renderPass);

		if (renderPass == VIEW_NORMAL)
			drawSSAOTexture();
	}
	
	if (ssao) {
		if (showOnlySSAO)
			drawTexture(ssaoTexture.colorTextureTargets[0], 0, textureProgram);
		else bindTexture(GL_TEXTURE9, ssaoTexture.colorTextureTargets[0]);
	}
}

void Renderer::drawWithDOF(){
	
	const mat4 projMatrix = cameraComponent->getProjMatrix();

	for(int i = 0; i < diaphragmPolygons; i++) {
		setClearFrameBuffer(fboList[i].framebufferId);
		
		const mat4 viewMatrix = cameraComponent->getViewMatrix(i, diaphragmPolygons, aperture);
		const mat4 lightProjMatrix = (*lights)[0]->getComponent<LightComponent>()->getProjMatrix();
		const mat4 lightViewMatrix = (*lights)[0]->getComponent<LightComponent>()->getViewMatrix();
		
		if (background != nullptr) background->getComponent<EnvironmentComponent>()->bindEnvMaps();

		if (renderShadows) {
			drawShadowMap(lightViewMatrix, lightProjMatrix);
		}

		setClearFrameBuffer(fboList[i].framebufferId);

		drawFromCamera(projMatrix, viewMatrix, lightViewMatrix, lightProjMatrix,
			STANDARD);
	}
	
	setClearFrameBuffer(0);
	glUseProgram(dofProgram);
	for (int i = 0; i < diaphragmPolygons; i++) {
		bindTexture(GL_TEXTURE0 + i, fboList[i].colorTextureTargets[0]);
	}
	
	labhelper::drawFullScreenQuad();

	glUseProgram(0);
	CHECK_GL_ERROR();
}
