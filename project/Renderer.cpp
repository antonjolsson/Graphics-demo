#include "Renderer.h"

Renderer::Renderer(Engine* _engine, CameraComponent* _camera, const std::vector<RenderComponent*>& _renderComponents,
		const bool _showHitbox) : engine(_engine), camera(_camera), renderComponents(_renderComponents),
		showHitbox(_showHitbox) {
	
}

void Renderer::setRenderShadows(const bool _renderShadows) {
	renderShadows = _renderShadows;
}

void Renderer::draw() {
	
}
