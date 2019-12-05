#pragma once

#include <GL/glew.h>
#include <vector>
#include <glm/detail/type_vec3.hpp>
#include <glm/mat4x4.hpp>
#include <algorithm>
#include "labhelper.h"

struct Particle
{
	float lifetime;
	float life_length;
	glm::vec3 velocity;
	glm::vec3 pos;
};

class ParticleSystem
{
private:
	// Members
	GLuint particleSysVAO, posBuffer;
	// Methods
	void updateReducedData(const glm::mat4& viewMatrix);
	void kill(int id);
	void spawn(Particle particle);
	void process_particles(float dt);
	void uploadToGPU(void);
	void render(void);
	void dummySpawn(void);
public:
	// Members
	std::vector<Particle> particles;
	std::vector<glm::vec4> reducedData;
	int max_size;
	// Ctor/Dtor
	ParticleSystem() : max_size(0)
	{
	}
	explicit ParticleSystem(int size);
	~ParticleSystem()
	{
	}
	void update(const glm::mat4& viewMatrix, float dt);
};


