#include "Renderer.h"
#include <glm/gtx/transform.hpp>

#include "EnvironmentComponent.h"
#include "LightComponent.h"
#include "RenderComponent.h"
#include "Ship.h"

void Renderer::setShadowMapProgram(const GLuint _shadowMapProgram) {
	shadowMapProgram = _shadowMapProgram;
}

Renderer::Renderer(InputHandler* _engine, GameObject* _camera, std::vector<RenderComponent*>* _renderComponents, 
                   const bool _showHitbox, const int _winWidth, const int _winHeight, std::vector<GameObject*>* _lights,
                   Ship* _ship, GameObject* _background, GameObject* _landingPad) :
	engine(_engine), camera(_camera), renderComponents(_renderComponents), showHitbox(_showHitbox), winWidth(_winWidth),
	winHeight(_winHeight), lights(_lights) {
	ship = _ship;
	landingPad = _landingPad;
	background = _background;
	cameraComponent = camera->getComponent<CameraComponent>();
	glEnable(GL_DEPTH_TEST); // enable Z-buffering
	glEnable(GL_CULL_FACE);  // enables backface culling
	glActiveTexture(GL_TEXTURE0);
}

void Renderer::setRenderShadows(const bool _renderShadows) {
	renderShadows = _renderShadows;
	if (renderShadows) {
		shadowMap = new ShadowMap(shadowMapProgram);
	}
}

void setMatrixUniforms(const GLuint _currentShaderProgram, const mat4& _viewMatrix, const mat4& _projectionMatrix, 
                       const mat4 _modelMatrix) {
	labhelper::setUniformSlow(_currentShaderProgram, "modelViewProjectionMatrix",
		_projectionMatrix * _viewMatrix * _modelMatrix);
	labhelper::setUniformSlow(_currentShaderProgram, "modelViewMatrix", _viewMatrix * _modelMatrix);
	labhelper::setUniformSlow(_currentShaderProgram, "normalMatrix",
		inverse(transpose(_viewMatrix * _modelMatrix)));
}

void Renderer::drawScene(const GLuint _shaderProgram, const mat4 _viewMatrix, const mat4 _projMatrix, const mat4 _lightViewMatrix,
                         const mat4 _lightProjMatrix) const {
	const vec4 viewSpaceLightPosition = _viewMatrix * vec4((*lights)[0]->getTransform().position, 1.0f);
	for (auto renderComponent : *renderComponents) {
		const GLuint compShaderProgram = _shaderProgram == 0 ? renderComponent->getShaderProgram() : _shaderProgram;
		glUseProgram(compShaderProgram);
		setLightUniforms(compShaderProgram, _viewMatrix, _lightViewMatrix, _lightProjMatrix,
			viewSpaceLightPosition);
		setMatrixUniforms(compShaderProgram, _viewMatrix, _projMatrix, renderComponent->getModelMatrix());

		// What's this doing?
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D,shadowMap->getShadowMapFB().depthBuffer);

		renderComponent->render(compShaderProgram);
	}
}

void Renderer::drawShadowMap(const mat4 _lightViewMatrix, const mat4 _lightProjMatrix) const {
	shadowMap->draw();
	drawScene(shadowMapProgram, _lightViewMatrix, _lightProjMatrix, _lightViewMatrix,
		_lightProjMatrix);
	//static labhelper::Model* landingpadModel = labhelper::loadModelFromOBJ("../scenes/landingpad.obj"); // TODO: REMOVE
	labhelper::Material& screen = landingPad->getComponent<ModelRenderComponent>()->getModel()->m_materials[8];
	screen.m_emission_texture.gl_id = shadowMap->getShadowMapFB().colorTextureTargets[0];

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
}

void Renderer::drawFromCamera(const mat4 _projMatrix, const mat4 _viewMatrix, const mat4 _lightViewMatrix, 
	const mat4 _lightProjMatrix) const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, winWidth, winHeight);
	glClearColor(0.2f, 0.2f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (background != nullptr) background->getComponent<EnvironmentComponent>()->draw(_viewMatrix, _projMatrix, camera);
	drawScene(0, _viewMatrix, _projMatrix, _lightViewMatrix,
		_lightProjMatrix);
}

void Renderer::draw() const {
	// TODO: abstract common fields and methods for matrices!
	
	const mat4 projMatrix = cameraComponent->getProjMatrix();
	const mat4 viewMatrix = cameraComponent->getViewMatrix();
	const mat4 lightProjMatrix = (*lights)[0]->getComponent<LightComponent>()->getProjMatrix();
	const mat4 lightViewMatrix = (*lights)[0]->getComponent<LightComponent>()->getViewMatrix();

	if (background != nullptr) background->getComponent<EnvironmentComponent>()->bindEnvMaps();

	if (renderShadows) 
		drawShadowMap(lightViewMatrix, lightProjMatrix);

	drawFromCamera(projMatrix, viewMatrix, lightViewMatrix, lightProjMatrix);
}
