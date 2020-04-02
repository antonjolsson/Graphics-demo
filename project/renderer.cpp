#include "renderer.h"

void Renderer::update(float _dt)
{
}

Renderer::Renderer(const GLuint _shaderProgram, labhelper::Model* _model)
{
	shaderProgram = -_shaderProgram;
	model = _model;
}
