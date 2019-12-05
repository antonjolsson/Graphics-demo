//#version 420 compatibility
//in float life;
//uniform float screen_x;
//uniform float screen_y;
//layout(binding = 0) uniform sampler2D colortexture;

//void main()
//{
	// Basse color.
	//gl_FragColor = texture2D(colortexture, gl_PointCoord);
	// Make it darker the older it is.
	//gl_FragColor.xyz *= (1.0 - life);
	// Make it fade out the older it is, also all particles have a
	// very low base alpha so they blend together.
	//gl_FragColor.w = gl_FragColor.w * (1.0 - pow(life, 4.0)) * 0.05;
//}

#version 420

precision highp float;

layout(location = 0) out vec4 fragmentColor;

void main()
{
	fragmentColor.rgb = vec3(0.f, 1.f, 0.f);
}