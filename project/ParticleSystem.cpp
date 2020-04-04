#include "ParticleSystem.h"

void ParticleSystem::initPosBuffer()
{
	glGenBuffers(1, &posBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)4 * max_size, nullptr, GL_STATIC_DRAW);
}

void ParticleSystem::initTexBuffer()
{
	glGenBuffers(1, &texBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texBuffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(fireTexCoords), fireTexCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(1);

	int w, h, comp;
	unsigned char* explImage = stbi_load("../scenes/explosion.png", &w, &h, &comp, STBI_rgb_alpha);
	glGenTextures(1, &texBuffer);
	glBindTexture(GL_TEXTURE_2D, texBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, explImage);
	free(explImage);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

ParticleSystem::ParticleSystem(const int size) : max_size(size)
{
	glGenVertexArrays(1, &particleSysVAO);
	glBindVertexArray(particleSysVAO);
	initPosBuffer();
	initTexBuffer();
}

vec3 ParticleSystem::getRandVelocity(void)
{
	const float theta = labhelper::uniform_randf(0.f, 2.f * M_PI);
	const float u = labhelper::uniform_randf(0.95f, 1.f);
	const float vel = labhelper::uniform_randf(10.f, 50.f);
	return glm::mat3(vel) * vec3(u, sqrt(1.f - u * u) * cosf(theta), sqrt(1.f - u * u) * sinf(theta));
}

void ParticleSystem::kill(int id)
{
	particles.erase(particles.begin() + id);
}

void ParticleSystem::spawn(Particle particle)
{
	if (particles.size() < max_size) particles.push_back(particle);
}

void ParticleSystem::processParticles(const float dt, glm::mat4& fighterModelMatrix)
{
	for (unsigned i = 0; i < particles.size(); ++i) {
		if (particles.at(i).lifetime > particles.at(i).life_length) kill(i);
	}
	for (auto& particle : particles)
	{
		particle.velocity += 0.01f * mat3(fighterModelMatrix) * getRandVelocity();;
		particle.pos += (dt * particle.velocity);
		particle.lifetime += dt;
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
	if (particles.empty()) return;
	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 16 * particles.size(), &reducedData[0]);
}

void ParticleSystem::render(void) {
	glBindVertexArray(particleSysVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texBuffer);
	//Enable shader program point size modulation.
	glEnable(GL_PROGRAM_POINT_SIZE);
	// Enable blending.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_POINTS, 0, reducedData.size());
	glDisable(GL_PROGRAM_POINT_SIZE);

	glBindVertexArray(0);
}

void ParticleSystem::update(const glm::mat4& viewMatrix, const float dt, glm::mat4& fighterModelMatrix, const bool accelerating)
{
	if (accelerating) spawnParticles(fighterModelMatrix);
	processParticles(dt, fighterModelMatrix);
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
