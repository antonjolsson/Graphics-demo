#version 420

precision highp float;

layout(location = 0) out vec4 fragmentColor;

void main()
{
	fragmentColor.rgb = vec3(0.f, 1.f, 0.f);
}