#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

layout(binding = 0) uniform sampler2D viewNormalTexture;
layout(binding = 1) uniform sampler2D randomRotTex;
layout(binding = 2) uniform sampler2D depthTexture;
// Test
layout(binding = 3) uniform sampler2D vsPositionTexture;
in vec3 viewSpacePosition;

uniform int numOfSamples;
uniform vec3 samples[50];
uniform mat4 inverseProjMatrix;
uniform mat4 projectionMatrix;
uniform float hemisphereRadius;

layout(location = 0) out vec4 fragmentColor;

float pi = 3.141592653589793;

void testDisplay(vec3 v){
	fragmentColor = vec4(v, 1);
}

void testDisplay(vec4 v){
	fragmentColor = v;
}

void testDisplay(float f){
	fragmentColor = vec4(f);
}

vec4 textureRect(in sampler2D tex, vec2 rectangleCoord)
{
	return texture(tex, rectangleCoord / textureSize(tex, 0));
}

vec3 homogenize(vec4 v) { 
	return vec3((1.0 / v.w) * v); 
}

// Computes one vector in the plane perpendicular to v
vec3 perpendicular(vec3 v)
{
	vec3 av = abs(v); 
	if (av.x < av.y)
		if (av.x < av.z) return vec3(0.0f, -v.z, v.y);
		else return vec3(-v.y, v.x, 0.0f);
	else
		if (av.y < av.z) return vec3(-v.z, 0.0f, v.x);
		else return vec3(-v.y, v.x, 0.0f);
}

mat3 createRotMatrix(float rotation) {
	vec3 v0 = vec3(cos(rotation), sin(rotation), 0);
	vec3 v1 = vec3(-sin(rotation), cos(rotation), 0);
	vec3 v2 = vec3(0, 0, 1);
	return mat3(v0, v1, v2);
}

mat4 rotationZ( in float angle ) {
	return mat4(	cos(angle),		-sin(angle),	0,	0,
			 		sin(angle),		cos(angle),		0,	0,
							0,				0,		1,	0,
							0,				0,		0,	1);
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


void main()
{
	// GOOD
	vec3 vsNormal = textureRect(viewNormalTexture, gl_FragCoord.xy).xyz;
	float fragmentDepth = textureRect(depthTexture, gl_FragCoord.xy).r;
	// GOOD
	vec2 texCoord = gl_FragCoord.xy / textureSize(viewNormalTexture, 0);

	float randomRot = 2 * pi * texture(randomRotTex, texCoord * textureSize(randomRotTex, 0)).x;
	mat3 zRotMatrix = createRotMatrix(randomRot);

	//test
	vsNormal = zRotMatrix * vsNormal;

	// GOOD
	// Normalized Device Coordinates (clip space)
	vec4 ndc = vec4(texCoord.x * 2.0 - 1.0, texCoord.y * 2.0 - 1.0, fragmentDepth * 2.0 - 1.0, 1.0);

	// Transform to view space
	vec3 vsPos = homogenize(inverseProjMatrix * ndc);

	vec3 vsTangent = perpendicular(vsNormal);
	vec3 vsBiTangent = cross(vsNormal, vsTangent);

	mat3 tbn = mat3(vsTangent, vsBiTangent, vsNormal); // local base

	
	//tbn = zRotMatrix * tbn;
	mat4 tbn4 = mat4(vec4(vsTangent, 1), vec4(vsBiTangent, 1), vec4(vsNormal, 1), vec4(0));
	tbn4 = rotationZ(randomRot) * tbn4;
	tbn = mat3(tbn4);

	int numVisibleSamples = 0; 
	int numValidSamples = 0; 

	for (int i = 0; i < numOfSamples; i++) {

		// Looks plausible
		// Project hemishere sample onto the local base
		vec3 hemiSample = tbn * samples[i];

		//Looks OK
		// compute view-space position of sample
		vec3 vsSamplePos = vsPos + hemiSample * hemisphereRadius;
		
		// compute the ndc-coords of the sample
		vec3 sampleCoordsNDC = homogenize(projectionMatrix * vec4(vsSamplePos, 1.0));

		// Looks OK (z little high)
		vec3 ssSampleCoords = vec3((sampleCoordsNDC.x + 1.0) / 2.0, (sampleCoordsNDC.y + 1.0) / 2.0, 
			(sampleCoordsNDC.z + 1.0) / 2.0);

		// Probably OK
		// Sample the depth-buffer at a texture coord based on the ndc-coord of the sample
		float blockerDepth = texture(depthTexture, ssSampleCoords.xy).r;

		// Find the view-space coord of the blocker
		vec3 vsBlockerPos = homogenize(inverseProjMatrix * vec4(sampleCoordsNDC.xy, blockerDepth * 2.0 - 1.0, 1.0));
		//vec3 vsBlockerPos = vec3(inverseProjMatrix * vec4(homogenize(vec4(ssSampleCoords.xy, blockerDepth * 2.0 - 1.0, 1.0)),1.0));

		// Wrong?
		testDisplay(distance(vsBlockerPos, vsSamplePos) / 100);

		//testDisplay(blockerDepth);
		//testDisplay(sampleCoords.z);

		testDisplay(abs(vsSamplePos.z) / 20);
		// Check that the blocker is closer than hemisphere_radius to vs_pos
		// (otherwise skip this sample)
		if (length(vsBlockerPos - vsPos) >= hemisphereRadius)
			continue;

		// Check if the blocker pos is closer to the camera than our fragment
		if (length(vsBlockerPos) >= length(vsSamplePos))
			numVisibleSamples += 1;

		numValidSamples += 1;
	}

	float hemisphericalVisibility = float(numVisibleSamples) / float(numValidSamples);

	if (numValidSamples == 0) hemisphericalVisibility = 1.0;

	testDisplay(hemisphericalVisibility);
}

