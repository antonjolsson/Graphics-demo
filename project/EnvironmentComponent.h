#pragma once
#include <GL/glew.h>
#include <glm/mat4x2.hpp>


#include "component.h"

class EnvironmentComponent : public Component {

	const std::string ENVMAP_BASE_NAME = "001";
	float environmentMultiplier = 2.5f; // 1.5f - original code
	GLuint environmentMap{};
public:
	std::string getEnvmapBaseName() const;
	float getEnvironmentMultiplier() const;
	GLuint getEnvironmentMap() const;
	void setEnvironmentMap(GLuint _environmentMap);
	void setIrradianceMap(GLuint _irradianceMap);
	void setReflectionMap(GLuint _reflectionMap);
	GLuint getIrradianceMap() const;
	GLuint getReflectionMap() const;
	void bindEnvMaps() const;
	void draw(glm::mat4 _viewMatrix, glm::mat4 _projMatrix, GameObject* _camera) const;
	GLuint environmentProgram;
private:
	GLuint irradianceMap{};
	GLuint reflectionMap{};
	
public:

	EnvironmentComponent(GameObject* _background, GLuint _environmentProgram);
};
