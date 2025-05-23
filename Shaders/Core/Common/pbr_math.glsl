#ifndef PBR_MATH_GLSL
#define PBR_MATH_GLSL

#include "constants.glsl" // Include common constants

// Isotropic GGX/Trowbridge-Reitz Normal Distribution Function (NDF)
// Calculates the statistical distribution of microfacet normals.
float DistributionGGX(vec3 N, vec3 H, float roughness) {
	float a      = roughness * roughness; // Perceptual roughness to roughness^2
	float a2     = a * a;                 // Roughness^4
	float NdotH  = max(dot(N, H), 0.0);   // Angle between normal and halfway vector
	float NdotH2 = NdotH * NdotH;
	float nom    = a2;
	// Denominator of the GGX NDF formula
	float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
	denom        = PI * denom * denom;
	return nom / max(denom, EPSILON); // Prevent division by zero
}

// Schlick's approximation for the Geometry Function (part of the Smith model)
// Approximates how much microfacets shadow/mask each other for direct lighting.
float GeometrySchlickGGX(float NdotV, float roughness) {
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Schlick's k for direct lighting
	float nom   = NdotV;
	// Denominator of the Schlick-GGX formula
	float denom = NdotV * (1.0 - k) + k;
	return nom / max(denom, EPSILON); // Prevent division by zero
}

// Smith's Geometry Function (combines view and light directions) - Isotropic
// Calculates the combined shadowing/masking effect for both view and light paths.
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0); // Angle between normal and view vector
	float NdotL = max(dot(N, L), 0.0); // Angle between normal and light vector
	float ggxV = GeometrySchlickGGX(NdotV, roughness); // Geometry term for view direction
	float ggxL = GeometrySchlickGGX(NdotL, roughness); // Geometry term for light direction
	return ggxV * ggxL; // Combined geometry factor G = G1(V) * G1(L)
}

// Fresnel Schlick approximation
// Calculates the ratio of light reflected vs. refracted at a surface interface.
// F0 is the reflectance at normal incidence (0 degrees).
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	// cosTheta is typically max(dot(H, V), 0.0)
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Fresnel Schlick approximation adjusted for roughness
// Modifies F0 based on roughness for a more realistic appearance on rough surfaces.
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
	// Interpolate between F0 and max(vec3(1.0 - roughness), F0) based on angle
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Anisotropic GGX Normal Distribution Function (NDF)
// Handles roughness differently along tangent (T) and bitangent (B) directions.
// alpha_x: roughness along T, alpha_y: roughness along B
float DistributionGGX_Aniso(vec3 N, vec3 H, vec3 T, vec3 B, float alpha_x, float alpha_y) {
	float dotHT = dot(H, T); // Projection of H onto T
	float dotHB = dot(H, B); // Projection of H onto B
	float dotHN = max(dot(H, N), 0.0); // Projection of H onto N

	float a = dotHT / alpha_x;
	float b = dotHB / alpha_y;
	// Denominator term based on projections and roughness values
	float denom_term = a*a + b*b + dotHN*dotHN;

	if (denom_term < EPSILON) return 0.0; // Avoid division by zero or near-zero

	// Final denominator includes normalization factors
	float denom = PI * alpha_x * alpha_y * denom_term * denom_term;

	return 1.0 / max(denom, EPSILON); // Prevent division by zero
}


// Smith's Geometry Function (G1 term) for Anisotropic GGX (Heitz 2014)
// Calculates shadowing/masking for a single direction (V or L).
float GeometrySmith_Aniso(vec3 N, vec3 V, vec3 T, vec3 B, float alpha_x, float alpha_y) {
	float dotVT = dot(V, T); // Projection of V onto T
	float dotVB = dot(V, B); // Projection of V onto B
	float dotVN = max(dot(V, N), 0.0); // Projection of V onto N

	if (dotVN < EPSILON) return 0.0; // Avoid division by zero if V is orthogonal to N

	// Calculate squared terms weighted by anisotropic roughness
	float variance_term_sq = (dotVT*dotVT) * (alpha_x*alpha_x) + (dotVB*dotVB) * (alpha_y*alpha_y);
	if (variance_term_sq < EPSILON) return 1.0; // Avoid division by zero if V is aligned with N

	// Calculate Lambda based on Heitz's derivation (Eq. 80)
	float tanThetaVSq_inv = (dotVN * dotVN) / variance_term_sq;
	float lambda = (-1.0 + sqrt(max(1.0 + tanThetaVSq_inv, 0.0))) / 2.0; // Ensure sqrt argument is non-negative

	// Smith G1 term: Visibility function for one direction
	float G1 = 1.0 / (1.0 + lambda);

	return G1;
}

// Combined Smith Geometry Function for Anisotropic GGX
// Combines the G1 terms for both view (V) and light (L) directions.
float GeometrySmithCombined_Aniso(vec3 N, vec3 V, vec3 L, vec3 T, vec3 B, float alpha_x, float alpha_y) {
	// Smith G = G1(V) * G1(L)
	float G_V = GeometrySmith_Aniso(N, V, T, B, alpha_x, alpha_y); // G1 for view direction
	float G_L = GeometrySmith_Aniso(N, L, T, B, alpha_x, alpha_y); // G1 for light direction
	return G_V * G_L; // Combined geometry factor
}


#endif // PBR_MATH_GLSL
