#ifndef SHADOW_GLSL
#define SHADOW_GLSL

// Shadow Calculation
// fragPosLightSpace: Fragment position in light's clip space
// shadowMap: Sampler for the shadow map texture
// NdotL: Dot product between normal and light direction (for bias)
float CalculateShadow(vec4 fragPosLightSpace, sampler2D shadowMap, float NdotL) {
    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Get current depth
    float currentDepth = projCoords.z;

    // Check if fragment is outside the light's frustum (near/far plane)
    // Note: projCoords.z is in [0,1] range after transformation.
    // If it's outside this range (e.g., > 1.0), it's outside the far plane.
    if(currentDepth > 1.0) return 0.0; // Not shadowed, but outside light range

    // Shadow bias to prevent shadow acne
    // Bias is larger for surfaces facing away from the light
    float bias = max(0.05 * (1.0 - NdotL), 0.005);

    // PCF - Simple 4-tap example for smoother shadows
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0); // Size of one texel
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0; // 1.0 if shadowed
        }
    }
    shadow /= 9.0; // Average the results

    // Return light contribution factor (1.0 = fully lit, 0.0 = fully shadowed)
    return 1.0 - shadow;
}

#endif // SHADOW_GLSL
