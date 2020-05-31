#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

in vec3 viewSpaceNormal;
layout(location = 0) out vec4 fragmentColor;
layout(location = 1) out vec4 depth;

void main()
{
	fragmentColor = vec4(normalize(viewSpaceNormal), 1);
	depth = vec4(vec3(gl_FragCoord.z), 1);
}