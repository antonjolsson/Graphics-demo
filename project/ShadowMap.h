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

	FboInfo shadowMapFB;
public:
	FboInfo getShadowMapFB() const;
private:
	int shadowMapResolution = 1024;
	int shadowMapClampMode = ClampMode::BORDER;
	bool shadowMapClampBorderShadowed = false;
	bool usePolygonOffset = true;
public:
	bool usesPolygonOffset() const;
private:
	bool useSoftFalloff = false;
	bool useHardwarePCF = true;
	float polygonOffsetFactor = 5.83f;
	float polygonOffsetUnits = 58.3f;

	GLuint shadowMapProgram;
	
public:

	ShadowMap(GLuint _shadowMapProgram);
	void draw();
};

#endif

