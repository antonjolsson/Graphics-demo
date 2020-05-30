#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

///////////////////////////////////////////////////////////////////////////////
// Texture
///////////////////////////////////////////////////////////////////////////////
layout(binding = 1) uniform sampler2D colorTexture;
uniform int has_diffuse_texture;

///////////////////////////////////////////////////////////////////////////////
// Material
///////////////////////////////////////////////////////////////////////////////
uniform vec3 material_color;
uniform float material_reflectivity;
uniform float material_metalness;
uniform float material_fresnel;
uniform float material_shininess;
uniform float material_emission;

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
uniform vec3 black_color = vec3(0);
uniform float point_light_intensity_multiplier = 50.0;
layout(binding = 10) uniform sampler2DShadow shadowMapTex;

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
in vec4 shadowMapCoord;

in vec3 worldPosition;

///////////////////////////////////////////////////////////////////////////////
// Input uniform variables
///////////////////////////////////////////////////////////////////////////////
uniform mat4 viewInverse;
uniform vec3 viewSpaceLightPosition;
uniform vec3 viewSpaceLightDir;
uniform float spotOuterAngle;
uniform float spotInnerAngle;

uniform bool fog;
uniform vec3 fogColor;
uniform float fogDensity;

uniform float depthRange;

uniform mat4 prevVPMatrix;

///////////////////////////////////////////////////////////////////////////////
// Output color
///////////////////////////////////////////////////////////////////////////////
layout(location = 0) out vec4 fragmentColor;

///////////////////////////////////////////////////////////////////////////////
// Motion blur
///////////////////////////////////////////////////////////////////////////////
vec3 velocity;

float getFresnel(vec3 wh, vec3 wi, vec3 wo) {
	return material_fresnel + (1 - material_fresnel) * pow(1 - dot(wh, wi), 5);
}

float highlightMultiplier = 2;

vec3 getMatColor() {
	return (has_diffuse_texture == 1) ? texture(colorTexture, texCoord).xyz : material_color;
}

vec3 calculateDirectIllumiunation(vec3 wo, vec3 n)
{
	vec3 direct_illum = point_light_color;
	
	float d = distance(viewSpaceLightPosition, viewSpacePosition);
	vec3 wi = normalize(viewSpaceLightPosition - viewSpacePosition);
	if (dot(n, wi) <= 0) return black_color;
	vec3 li = point_light_intensity_multiplier * point_light_color * (1.f / (d * d));
	
	vec3 diffuse_term = getMatColor() * (1.f / PI) * dot(n, wi) * li;

	vec3 wh = normalize(wi + wo);
	float f = getFresnel(wh, wi, wo);
	float distrib = dot(n, wh) < 0 ? 0 : (material_shininess + 2.f) / (PI * 2.f) * pow(dot(n, wh), material_shininess); 

	float g1 = 2.f * dot(n, wh) * dot(n, wo) / dot(wo, wh);
	float g2 = 2.f * dot(n, wh) * dot(n, wi) / dot(wo, wh);
	float g = min(1.f, min(g1, g2));

	float brdf = f * distrib * g / (4 * dot(n, wo) * dot(n, wi));

	vec3 dielectric_term = brdf * dot(n, wi) * li + (1 - f) * diffuse_term;
	vec3 metal_term = brdf * getMatColor() * dot(n, wi) * li;
	vec3 microfacet_term = material_metalness * metal_term + (1 - material_metalness) * dielectric_term;

	return highlightMultiplier * (material_reflectivity * microfacet_term + (1 - material_reflectivity) * diffuse_term); // Added multiplier here
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

vec3 calculateIndirectIllumination(vec3 wo, vec3 n)
{
	vec3 nws = (viewInverse * vec4(n, 0)).xyz;

	vec2 lookup = getSphericalCoords(nws);
	vec3 irradiance = (environment_multiplier * texture(irradianceMap, lookup)).xyz;

	vec3 diffuse_term = getMatColor() * (1.0 / PI) * irradiance;

	vec3 wi = -(viewInverse * vec4(reflect(wo, n), 0)).xyz;
	lookup = getSphericalCoords(wi);
	float roughness = pow(2.0 / (material_shininess + 2), 0.25f);
	vec3 li = environment_multiplier * textureLod(reflectionMap, lookup, roughness * 7.0).xyz;
	vec3 wh = normalize(wi + wo);
	float f = getFresnel(wh, wi, wo);
	vec3 dielectric_term = f * li + (1 - f) * diffuse_term;
	vec3 metal_term = f * getMatColor() * li;
	vec3 microfacet_term = material_metalness * metal_term + (1 - material_metalness) * dielectric_term;

	return material_reflectivity * microfacet_term + (1 - material_reflectivity) * diffuse_term;
}

vec3 getViewSpaceDepth() {
	return vec3(clamp(1 - (1.f / depthRange) * (viewSpacePosition.z + depthRange), 0, 1));
}

vec4 addFog(vec3 shading, float depth) {
	float invHeight = clamp(1.f / (0.4 * (worldPosition.y + 0.01)), 0, 1);
	float heightAdjustedDepth = clamp((0.2f * depth + 1.8f * invHeight) / 2.f, 0, 1);
	return fogDensity * heightAdjustedDepth * vec4(fogColor, 1) + (1 - fogDensity * heightAdjustedDepth) * vec4(shading, 1);
}

// Doesn't work... yet!
void computePixelVelocity() {
	vec3 previousScreenPos = (prevVPMatrix * vec4(worldPosition, 1)).xyz;
	velocity = gl_FragCoord.xyz - previousScreenPos;

}

void main()
{
	//computePixelVelocity();

	float visibility = textureProj( shadowMapTex, shadowMapCoord );
	float attenuation = 1.0;

	vec3 posToLight = normalize(viewSpaceLightPosition - viewSpacePosition);
	float cosAngle = dot(posToLight, -viewSpaceLightDir);

	vec3 wo = -normalize(viewSpacePosition);
	vec3 n = normalize(viewSpaceNormal);

	// Spotlight with hard border:
	
	float spotAttenuation = smoothstep(spotOuterAngle, spotInnerAngle, cosAngle);
	visibility *= spotAttenuation;

	// Direct illumination
	vec3 direct_illumination_term = visibility * calculateDirectIllumiunation(wo, n);

	// Indirect illumination
	vec3 indirect_illumination_term = calculateIndirectIllumination(wo, n);

	///////////////////////////////////////////////////////////////////////////
	// Add emissive term. If emissive texture exists, sample this term.
	///////////////////////////////////////////////////////////////////////////
	//vec3 emission_term = material_emission * material_color;
	vec3 emission_term = material_emission * getMatColor();
	if(has_emission_texture == 1)
	{
		emission_term = texture(emissiveMap, texCoord).xyz;
	}

	vec3 shading = direct_illumination_term + indirect_illumination_term + emission_term;

	//fragmentColor = vec4(velocity, 1);
	fragmentColor = fog ? addFog(shading, getViewSpaceDepth().z) : vec4(shading, 1.f);

	return;
}
