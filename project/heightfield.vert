#version 420
///////////////////////////////////////////////////////////////////////////////
// Input vertex attributes
///////////////////////////////////////////////////////////////////////////////
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoordIn;

layout(binding = 0) uniform sampler2D hfTexture;

///////////////////////////////////////////////////////////////////////////////
// Input uniform variables
///////////////////////////////////////////////////////////////////////////////

uniform mat4 normalMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 lightMatrix;

///////////////////////////////////////////////////////////////////////////////
// Output to fragment shader
///////////////////////////////////////////////////////////////////////////////
out vec2 texCoord;
out vec3 viewSpacePosition;
out vec3 viewSpaceNormal;
out vec4 shadowMapCoord;

const float HEIGHT_SCALING = 1.f / 5;

vec3 getHeightAdjustedPos(vec2 offset) {
	return vec3(position.x + offset.x, texture(hfTexture, vec2(texCoordIn.x + offset.x, texCoordIn.y + offset.y)).r * HEIGHT_SCALING, 
		position.z + offset.y);
}

vec3 getEstnormal() {
	vec3 posA = getHeightAdjustedPos(vec2(1, -1));
	vec3 posB = getHeightAdjustedPos(vec2(-1, 1));
	vec3 posC = getHeightAdjustedPos(vec2(1, 1));
	return normalize(cross(posB - posA, posC - posA));
}

void main()
{
	vec3 adjustedPosition = getHeightAdjustedPos(vec2(0, 0));
	gl_Position = modelViewProjectionMatrix * vec4(adjustedPosition, 1.0);
	texCoord = texCoordIn;
	shadowMapCoord = lightMatrix * vec4(viewSpacePosition, 1.f);
	vec3 normalIn = getEstnormal();
	viewSpaceNormal = (normalMatrix * vec4(normalIn, 0.0)).xyz;
	viewSpacePosition = (modelViewMatrix * vec4(position, 1.0)).xyz;
}
