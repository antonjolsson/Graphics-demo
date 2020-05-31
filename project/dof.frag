#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

layout(binding = 0) uniform sampler2D frameBufferTexture0;
layout(binding = 1) uniform sampler2D frameBufferTexture1;
layout(binding = 2) uniform sampler2D frameBufferTexture2;
layout(binding = 3) uniform sampler2D frameBufferTexture3;
layout(binding = 4) uniform sampler2D frameBufferTexture4;
layout(binding = 5) uniform sampler2D frameBufferTexture5;
layout(binding = 6) uniform sampler2D frameBufferTexture6;
layout(binding = 7) uniform sampler2D frameBufferTexture7;
layout(binding = 8) uniform sampler2D frameBufferTexture8;
layout(binding = 9) uniform sampler2D frameBufferTexture9;

layout(location = 0) out vec4 fragmentColor;

/**
* Helper function to sample with pixel coordinates, e.g., (511.5, 12.75)
* This functionality is similar to using sampler2DRect.
* TexelFetch only work with integer coordinates and do not perform bilinerar filtering.
*/
vec4 textureRect(in sampler2D tex, vec2 rectangleCoord)
{
	return texture(tex, rectangleCoord / textureSize(tex, 0));
}

void main() {
	fragmentColor = (textureRect(frameBufferTexture0, gl_FragCoord.xy) + textureRect(frameBufferTexture1, gl_FragCoord.xy) + 
	textureRect(frameBufferTexture2, gl_FragCoord.xy) + textureRect(frameBufferTexture3, gl_FragCoord.xy) + 
	textureRect(frameBufferTexture4, gl_FragCoord.xy) + textureRect(frameBufferTexture5, gl_FragCoord.xy) + 
	textureRect(frameBufferTexture6, gl_FragCoord.xy) + textureRect(frameBufferTexture7, gl_FragCoord.xy) +
	textureRect(frameBufferTexture6, gl_FragCoord.xy) + textureRect(frameBufferTexture7, gl_FragCoord.xy)) / 10;
}