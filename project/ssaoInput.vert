#version 420

layout(location = 0) in vec3 position;
out vec2 texCoord;
uniform vec3 hemisphereSamples[16];

//Test
out vec3 viewSpacePosition;
uniform mat4 modelViewMatrix;

void main()
{
	gl_Position = vec4(position, 1);
	texCoord = position.xy;
	// Test
	viewSpacePosition = (modelViewMatrix * vec4(position, 1.0)).xyz;
}
