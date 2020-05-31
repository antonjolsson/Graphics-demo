#include "ShadowMap.h"

#include <glm/detail/type_vec4.hpp>

Fbo ShadowMap::getShadowMapFB() const {
	return shadowMapFB;
}

bool ShadowMap::usesPolygonOffset() const {
	return usePolygonOffset;
}

ShadowMap::ShadowMap(GLuint _shadowMapProgram) {
	shadowMapProgram = _shadowMapProgram;
	
	shadowMapFB.resize(shadowMapResolution, shadowMapResolution);
	
	glBindTexture(GL_TEXTURE_2D, shadowMapFB.depthBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
}

void ShadowMap::draw() {
	glActiveTexture(GL_TEXTURE0);
	
	if (shadowMapFB.width != shadowMapResolution || shadowMapFB.height != shadowMapResolution)
	{
		shadowMapFB.resize(shadowMapResolution, shadowMapResolution);
	}
	
	if (shadowMapClampMode == ClampMode::EDGE)
	{
		glBindTexture(GL_TEXTURE_2D, shadowMapFB.depthBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	if (shadowMapClampMode == ClampMode::BORDER)
	{
		glBindTexture(GL_TEXTURE_2D, shadowMapFB.depthBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glm::vec4 border(shadowMapClampBorderShadowed ? 0.f : 1.f);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &border.x);
	}

	if (usePolygonOffset)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(polygonOffsetFactor, polygonOffsetUnits);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFB.framebufferId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, shadowMapFB.width, shadowMapFB.height);
}

