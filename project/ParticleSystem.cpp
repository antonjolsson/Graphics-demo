#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(int size) : max_size(size)
{
	glGenVertexArrays(1, &particleSysVAO);
	glBindVertexArray(particleSysVAO);
	glGenBuffers(1, &posBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, max_size, nullptr, GL_STATIC_DRAW);
	dummySpawn();
}

void ParticleSystem::dummySpawn(void)
{
	for (size_t i = 0; i < 10000; i++)
	{
		Particle particle;
		const float theta = labhelper::uniform_randf(0.f, 2.f * M_PI);
		const float u = labhelper::uniform_randf(-1.f, 1.f);
		particle.pos = glm::mat3(10.0f) * glm::vec3(sqrt(1.f - u * u) * cosf(theta), u, sqrt(1.f - u * u) * sinf(theta));
		particles.push_back(particle);
	}
}

void ParticleSystem::kill(int id)
{
}

void ParticleSystem::spawn(Particle particle)
{
}

void ParticleSystem::process_particles(float dt)
{
	for (unsigned i = 0; i < particles.size(); ++i) {
		// Kill dead particles!
	}
	for (unsigned i = 0; i < particles.size(); ++i) {
		// Update alive particles!
	}
}

void ParticleSystem::uploadToGPU(void) {
	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 16 * particles.size(), &reducedData[0]);
}

void ParticleSystem::render(void) {
	glBindVertexArray(particleSysVAO);
	glDrawArrays(GL_POINTS, 0, reducedData.size());
}

void ParticleSystem::update(const glm::mat4& viewMatrix, float dt)
{
	// spawn(Particle particle);
	process_particles(dt);
	updateReducedData(viewMatrix);
	uploadToGPU();
	render();
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
