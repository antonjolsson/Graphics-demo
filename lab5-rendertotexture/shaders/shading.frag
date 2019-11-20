#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

///////////////////////////////////////////////////////////////////////////////
// Material
///////////////////////////////////////////////////////////////////////////////
uniform vec3 material_color;
uniform float material_reflectivity;
uniform float material_metalness;
uniform float material_fresnel;
uniform float material_shininess;
uniform float material_emission;

uniform int has_color_texture;
layout(binding = 0) uniform sampler2D colorMap;
uniform int has_emission_texture;
layout(binding = 5) uniform sampler2D emissiveMap;

///////////////////////////////////////////////////////////////////////////////
// Environment
///////////////////////////////////////////////////////////////////////////////
layout(binding = 6) uniform sampler2D environmentMap;
layout(binding = 7) uniform sampler2D irradianceMap;
layout(binding = 8) uniform sampler2D reflectionMap;
uniform float environment_multiplier;

///////////////////////////////////////////////////////////////////////////////
// Light source
///////////////////////////////////////////////////////////////////////////////
uniform vec3 point_light_color = vec3(1.0, 1.0, 1.0);
uniform float point_light_intensity_multiplier = 50.0;
uniform vec3 black_color = vec3(0.0, 0.0, 0.0);

///////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////
#define PI 3.14159265359

///////////////////////////////////////////////////////////////////////////////
// Input varyings from vertex shader
///////////////////////////////////////////////////////////////////////////////
in vec2 texCoord;
in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;

///////////////////////////////////////////////////////////////////////////////
// Input uniform variables
///////////////////////////////////////////////////////////////////////////////
uniform mat4 viewInverse;
uniform vec3 viewSpaceLightPosition;

///////////////////////////////////////////////////////////////////////////////
// Output color
///////////////////////////////////////////////////////////////////////////////
layout(location = 0) out vec4 fragmentColor;


float getFresnel(vec3 wh, vec3 wi, vec3 wo) {
	return material_fresnel + (1 - material_fresnel) * pow(1 - dot(wh, wi), 5);
}

vec3 calculateDirectIllumiunation(vec3 wo, vec3 n, vec3 base_color)
{
	vec3 direct_illum = base_color;
	
	float d = distance(viewSpaceLightPosition, viewSpacePosition);
	vec3 wi = normalize(viewSpaceLightPosition - viewSpacePosition);
	if (dot(n, wi) <= 0) return black_color;
	vec3 li = point_light_intensity_multiplier * point_light_color * (1.f / (d * d));
	
	vec3 diffuse_term = material_color * (1.f / PI) * dot(n, wi) * li;

	vec3 wh = normalize(wi + wo);
	float f = getFresnel(wh, wi, wo);
	float distrib = dot(n, wh) < 0 ? 0 : (material_shininess + 2.f) / (PI * 2.f) * pow(dot(n, wh), material_shininess); 

	float g1 = 2.f * dot(n, wh) * dot(n, wo) / dot(wo, wh);
	float g2 = 2.f * dot(n, wh) * dot(n, wi) / dot(wo, wh);
	float g = min(1.f, min(g1, g2));

	float brdf = f * distrib * g / (4 * dot(n, wo) * dot(n, wi));

	vec3 dielectric_term = brdf * dot(n, wi) * li + (1 - f) * diffuse_term;
	vec3 metal_term = brdf * material_color * dot(n, wi) * li;
	vec3 microfacet_term = material_metalness * metal_term + (1 - material_metalness) * dielectric_term;

	return material_reflectivity * microfacet_term + (1 - material_reflectivity) * diffuse_term;
}

vec2 getSphericalCoords(vec3 v){
	// Calculate the spherical coordinates of the direction

	float theta = acos(max(-1.0f, min(1.0f, v.y)));
	float phi = atan(v.z, v.x);
	if(phi < 0.0f)
	{
		phi = phi + 2.0f * PI;
	}

	return vec2(phi / (2.0 * PI), theta / PI);
}

vec3 calculateIndirectIllumination(vec3 wo, vec3 n, vec3 base_color)
{
	vec3 nws = (viewInverse * vec4(n, 0)).xyz;

	vec2 lookup = getSphericalCoords(nws);
	vec3 irradiance = (environment_multiplier * texture(irradianceMap, lookup)).xyz;

	vec3 diffuse_term = material_color * (1.0 / PI) * irradiance;

	//return diffuse_term;

	vec3 wi = -(viewInverse * vec4(reflect(wo, n), 0)).xyz;
	lookup = getSphericalCoords(wi);
	float roughness = pow(2.0 / (material_shininess + 2), 0.25f);
	vec3 li = environment_multiplier * textureLod(reflectionMap, lookup, roughness * 7.0).xyz;
	vec3 wh = normalize(wi + wo);
	float f = getFresnel(wh, wi, wo);
	vec3 dielectric_term = f * li + (1 - f) * diffuse_term;
	vec3 metal_term = f * material_color * li;
	vec3 microfacet_term = material_metalness * metal_term + (1 - material_metalness) * dielectric_term;

	return material_reflectivity * microfacet_term + (1 - material_reflectivity) * diffuse_term;
}


void main()
{
	vec3 wo = -normalize(viewSpacePosition);
	vec3 n = normalize(viewSpaceNormal);

	vec3 base_color = material_color;
	if(has_color_texture == 1)
	{
		base_color *= texture(colorMap, texCoord).xyz;
	}

	// Direct illumination
	vec3 direct_illumination_term = calculateDirectIllumiunation(wo, n, base_color);

	// Indirect illumination
	vec3 indirect_illumination_term = calculateIndirectIllumination(wo, n, base_color);

	vec3 emission_term = material_emission * material_color;

	if(has_emission_texture == 1)
	{
		emission_term *= texture(emissiveMap, texCoord).xyz;
	}

	fragmentColor.xyz = direct_illumination_term + indirect_illumination_term + emission_term;
}
