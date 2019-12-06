#pragma once

#include <GL/glew.h>
#include <vector>
#include <glm/detail/type_vec3.hpp>
#include <glm/mat4x4.hpp>
#include <algorithm>
#include "labhelper.h"

using namespace glm;

struct Particle
{
	float lifetime;
	float life_length;
	vec3 velocity;
	vec3 pos;
};

static const uint SPAWNED_PARTICLES = 64;
static const uint PARTICLE_LIFE_LENGTH = 5;

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
	void spawnParticles(void);
	void uploadToGPU(void);
	void render(void);
	vec3 getRandVelocity(void);
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


