#pragma once
#include "component.h"
#include <GL/glew.h>
#include <glm/detail/type_mat.hpp>
#include <glm/glm.hpp>

class RenderComponent : public Component {

protected:
	
	glm::mat4 modelMatrix{0};
	GLuint shaderProgram = 0;
	
public:
	GLuint getShaderProgram() const;
	virtual void render(){}
	glm::mat4 getModelMatrix() const;;

};
