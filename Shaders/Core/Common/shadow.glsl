#ifndef SHADOW_GLSL
#define SHADOW_GLSL

// ============================================================================
// REQUIRED UNIFORMS & INPUTS (Must be provided by the including shader)
// ============================================================================
// uniform sampler2D shadowMap; // Sampler for the shadow map texture.
// in vec4 FragPosLightSpace;   // Fragment position transformed by the light's view-projection matrix.
// in vec3 Normal;              // World-space surface normal.
// uniform vec3 u_LightDir;    // Direction *towards* the light source (needed for bias).
// uniform vec3 u_ViewPos;     // Camera position (optional, potentially useful for other bias methods).

// ============================================================================
// CONSTANTS
// ============================================================================
// const float EPSILON = 1e-5; // Small value, potentially useful for bias calculations.

// ============================================================================
// SHADOW CALCULATION (Directional Light, Basic PCF)
// ============================================================================
// Calculates the shadow attenuation factor using simple Percentage-Closer Filtering (PCF).
// Returns 1.0 for fully lit fragments, 0.0 for fully shadowed fragments.
float CalculateShadow(sampler2D shadowMapSampler, vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
	// Perform perspective divide to get Normalized Device Coordinates (NDC) [-1, 1].
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	// Map NDC to texture coordinates [0, 1] for sampling the shadow map.
	projCoords = projCoords * 0.5 + 0.5;

	// Fragments outside the light's view frustum (far plane) are considered not shadowed.
	// Check the depth component (z) before sampling.
	if (projCoords.z > 1.0) {
		return 1.0; // Not shadowed.
	}

	// Calculate shadow bias to prevent "shadow acne" (self-shadowing artifacts).
	// Slope-scale bias: increases bias for surfaces nearly parallel to the light direction.
	// Clamps to a minimum bias value.
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	// Alternative: Fixed bias (simpler but less robust)
	// float bias = 0.005;

	// --- Percentage-Closer Filtering (PCF) ---
	// Sample the shadow map multiple times in a small neighborhood and average the results.
	float shadowFactor = 0.0; // Accumulator for shadow contribution (0 = lit, 1 = shadowed).
	vec2 texelSize = 1.0 / textureSize(shadowMapSampler, 0); // Calculate the size of a single texel.

	// Sample a 3x3 neighborhood around the projected coordinates.
	for (int x = -1; x <= 1; ++x) {
		for (int y = -1; y <= 1; ++y) {
			// Calculate sample coordinates for the current tap.
			vec2 sampleCoords = projCoords.xy + vec2(x, y) * texelSize;
			// Sample the depth stored in the shadow map at the tap location.
			float pcfDepth = texture(shadowMapSampler, sampleCoords).r;
			// Compare the current fragment's depth (with bias applied) to the sampled depth.
			// If the fragment is further from the light than the stored depth, it's shadowed for this tap.
			shadowFactor += (projCoords.z - bias) > pcfDepth ? 1.0 : 0.0;
		}
	}
	// Average the shadow contribution over the 9 samples.
	shadowFactor /= 9.0;

	// The result `shadowFactor` is the percentage of samples that were determined to be in shadow.
	// Return the lighting factor (1.0 - shadow percentage).
	return 1.0 - shadowFactor;
}

#endif // SHADOW_GLSL