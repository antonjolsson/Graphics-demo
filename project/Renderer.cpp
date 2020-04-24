#include "Renderer.h"

Renderer::Renderer(Engine* _engine, GameObject* _camera, const std::vector<RenderComponent*>& _renderComponents,
                   const bool _showHitbox, const int _winWidth, const int _winHeight) : engine(_engine), camera(_camera),
                    renderComponents(_renderComponents), showHitbox(_showHitbox), winWidth(_winWidth), winHeight(_winHeight) {
	cameraComponent = camera->getComponent<CameraComponent>();
	glEnable(GL_DEPTH_TEST); // enable Z-buffering
	glEnable(GL_CULL_FACE);  // enables backface culling
}

void Renderer::setRenderShadows(const bool _renderShadows) {
	renderShadows = _renderShadows;
}

void Renderer::draw() {
	const mat4 projMatrix = cameraComponent->getProjMatrix();
	const mat4 viewMatrix = cameraComponent->getViewMatrix();
}
