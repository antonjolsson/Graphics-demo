#version 420 compatibility
//#define DEBUG

in float life;
uniform float screen_x;
uniform float screen_y;
layout(binding = 0) uniform sampler2D colortexture;

void main()
{
	#ifndef DEBUG
	// Basse color.
	gl_FragColor = texture2D(colortexture, gl_PointCoord);
	// Make it darker the older it is.
	gl_FragColor.xyz *= (1.0 - 0.1 * life);
	// Make it fade out the older it is, also all particles have a
	// very low base alpha so they blend together.
	gl_FragColor.w *= (1.0 - pow(life, 1.0)) * 0.05;
	#endif

	#ifdef DEBUG
	gl_FragColor = vec4(0.f, 1.f, 0.f, 1.f);
	#endif
}