#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

uniform mat4 prevVPMatrix;
uniform mat4 inverseViewProjMatrix;
uniform mat4 viewProjectionMatrix;
uniform mat4 inverseProjMatrix;
uniform mat4 inverseViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 prevProjMatrix;

layout(binding = 0) uniform sampler2D colorTexture;
layout(binding = 1) uniform sampler2D depthTexture;

layout(location = 0) out vec4 fragmentColor;

int numSamples = 50;

vec4 textureRect(in sampler2D tex, vec2 rectangleCoord)
{
	return texture(tex, rectangleCoord / textureSize(tex, 0));
}

void testDisplay(float f){
	fragmentColor = vec4(f);
}

void testDisplay(vec3 v){
	fragmentColor = vec4(v, 1);
}

void testDisplay(vec4 v){
	fragmentColor = v;
}

vec3 homogenize(vec4 v) { 
	return vec3((1.0 / v.w) * v); 
}

void main()
{
	float depth = textureRect(depthTexture, gl_FragCoord.xy).r;
	vec2 texCoord = gl_FragCoord.xy / textureSize(colorTexture, 0);
	vec4 currentPosNDC = vec4(texCoord.x * 2.0 - 1.0, texCoord.y * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	//vec3 vsPos = vec3(inverseProjMatrix * ndc);
	vec3 wsPos = homogenize(inverseViewProjMatrix * currentPosNDC).xyz;
	vec4 previousPosNDC = vec4(homogenize(prevVPMatrix * vec4(wsPos, 1.0)), 1.0);

	vec2 velocity = (currentPosNDC - previousPosNDC).xy / 50.f;
	//vec3 velocity = 1000 * (homogenize(projectionMatrix * vec4(vsPos, 1)) - homogenize(prevProjMatrix * vec4(vsPos, 1.0))).xyz;

	vec3 finalColor = texture(colorTexture, texCoord).xyz;

	int skippedSamples = 0;

	for (int i = 0; i < numSamples; i++) {
		texCoord += velocity;
		if (texCoord.x > 1.0 || texCoord.x < -1.0 || texCoord.y > 1.0 || texCoord.y < -1.0) {
			skippedSamples++;
			continue;
		}
		finalColor += texture(colorTexture, texCoord).xyz;
	}

	float numSamples_f = float(numSamples - skippedSamples);

	finalColor /= numSamples_f;

	testDisplay(finalColor);

}