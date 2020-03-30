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

void main()
{
	vec3 adjustedPosition = vec3(position.x, texture(hfTexture, texCoordIn).r / 5, position.z);
	gl_Position = modelViewProjectionMatrix * vec4(adjustedPosition, 1.0);
	//gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
	texCoord = texCoordIn;
	shadowMapCoord = lightMatrix * vec4(viewSpacePosition, 1.f);
	vec3 normalIn = vec3 (1.f, 1.f, 0.f);
	viewSpaceNormal = (normalMatrix * vec4(normalIn, 0.0)).xyz;
	viewSpacePosition = (modelViewMatrix * vec4(position, 1.0)).xyz;
}
