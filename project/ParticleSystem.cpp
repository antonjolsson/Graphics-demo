#include "ParticleSystem.h"
#include <iostream>

ParticleSystem::ParticleSystem(int size) : max_size(size)
{
	glGenVertexArrays(1, &particleSysVAO);
	glBindVertexArray(particleSysVAO);
	glGenBuffers(1, &posBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) 4 * max_size, nullptr, GL_STATIC_DRAW);
	//dummySpawn();
}

vec3 ParticleSystem::getRandVelocity(void)
{
	const float theta = labhelper::uniform_randf(0.f, 2.f * M_PI);
	const float u = labhelper::uniform_randf(0.95f, 1.f);
	return glm::mat3(10.0f) * vec3(u, sqrt(1.f - u * u) * cosf(theta), sqrt(1.f - u * u) * sinf(theta));
}

void ParticleSystem::dummySpawn(void)
{
	for (size_t i = 0; i < 10000; i++)
	{
		Particle particle;
		particle.pos = getRandVelocity();
		particles.push_back(particle);
	}
}

void ParticleSystem::kill(int id)
{
	particles.erase(particles.begin() + id);
}

void ParticleSystem::spawn(Particle particle)
{
	if (particles.size() < max_size) particles.push_back(particle);
}

void ParticleSystem::process_particles(float dt)
{
	for (unsigned i = 0; i < particles.size(); ++i) {
		if (particles.at(i).lifetime > particles.at(i).life_length) kill(i);
	}
	for (unsigned i = 0; i < particles.size(); ++i) {
		particles.at(i).pos += dt * particles.at(i).velocity;
		particles.at(i).lifetime += dt;
	}
}

void ParticleSystem::spawnParticles(glm::mat4& fighterModelMatrix) {
	for (size_t i = 0; i < SPAWNED_PARTICLES; i++)
	{
		Particle particle;
		particle.pos = vec3(fighterModelMatrix * vec4(exhaustOffset, 1.f));
		particle.velocity = mat3(fighterModelMatrix) * getRandVelocity();
		particle.lifetime = 0;
		particle.life_length = PARTICLE_LIFE_LENGTH;
		spawn(particle);
	}
}

void ParticleSystem::uploadToGPU(void) {
	if (particles.size() == 0) return;
	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 16 * particles.size(), &reducedData[0]);
}

void ParticleSystem::render(void) {
	glBindVertexArray(particleSysVAO);
	glDrawArrays(GL_POINTS, 0, reducedData.size());
}

void ParticleSystem::update(const glm::mat4& viewMatrix, float dt, glm::mat4& fighterModelMatrix, bool accelerating)
{
	if (accelerating) spawnParticles(fighterModelMatrix);
	process_particles(dt);
	updateReducedData(viewMatrix);
	uploadToGPU();
	render();
}

void ParticleSystem::setExhaustOffset(glm::vec3& offset)
{
	exhaustOffset = offset;
}

void ParticleSystem::updateReducedData(const glm::mat4& viewMatrix)
{
	reducedData.clear();
	unsigned int activeParticles = particles.size();
	for each (Particle particle in particles)
	{
		glm::vec3 viewPosition = viewMatrix * glm::vec4(particle.pos, 1.0);
		reducedData.push_back(glm::vec4(viewPosition, particle.lifetime));
	}
	
	std::sort(reducedData.begin(), std::next(reducedData.begin(), activeParticles),
		[](const glm::vec4& lhs, const glm::vec4& rhs) { return lhs.z < rhs.z; });
}
