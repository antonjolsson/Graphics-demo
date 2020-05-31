#pragma once

#ifndef FBO_H
#define FBO_H

#include "fbo.h"

class ShadowMap {

	enum ClampMode
	{
		EDGE = 1,
		BORDER = 2
	};

	Fbo shadowMapFB;
public:
	Fbo getShadowMapFB() const;
private:
	int shadowMapResolution = 20000; // 1024
	int shadowMapClampMode = ClampMode::BORDER;
	bool shadowMapClampBorderShadowed = false;
	bool usePolygonOffset = true;
public:
	bool usesPolygonOffset() const;
private:
	bool useSoftFalloff = true;
	bool useHardwarePCF = true;
	float polygonOffsetFactor = 5.83f;
	float polygonOffsetUnits = 58.3f;

	GLuint shadowMapProgram;
	
public:

	ShadowMap(GLuint _shadowMapProgram);
	void draw();
};

#endif

