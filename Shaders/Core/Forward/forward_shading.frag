#version 450 core
#extension GL_ARB_separate_shader_objects : enable

// ============================================================================
// OUTPUT
// ============================================================================
out vec4 FragColor;

// ============================================================================
// INPUTS FROM VERTEX SHADER
// ============================================================================
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    mat3 TBN;
    vec4 FragPosLightSpace;
    vec3 WorldTangent;
    vec3 WorldBitangent;
} fs_in;

// ============================================================================
// INCLUDES (Common functions and constants)
// ============================================================================
#include "../Common/constants.glsl"
#include "../Common/lighting.glsl"
#include "../Common/pbr_math.glsl"
#include "../Common/shadow.glsl"

// ============================================================================
// TEXTURE SAMPLERS (Match MaterialPBR setup)
// ============================================================================
uniform sampler2D u_AlbedoMap;     // Unit 0
uniform sampler2D u_NormalMap;     // Unit 1
uniform sampler2D u_MetallicMap;   // Unit 2
uniform sampler2D u_RoughnessMap;  // Unit 3
uniform sampler2D u_AOMap;         // Unit 4
uniform sampler2D u_EmissiveMap;   // Unit 5 (Optional)
uniform sampler2D shadowMap;       // Unit (e.g., 11) for directional shadow

// ============================================================================
// MATERIAL UNIFORMS (Match MaterialPBR setup)
// ============================================================================
uniform vec3 u_AlbedoColor;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_AO;
uniform vec3 u_EmissiveColor; // Optional

uniform int u_HasAlbedoMap;
uniform int u_HasNormalMap;
uniform int u_HasMetallicMap;
uniform int u_HasRoughnessMap;
uniform int u_HasAOMap;
uniform int u_HasEmissiveMap; // Optional

// ============================================================================
// LIGHT UNIFORMS (Use structs from lighting.glsl)
// ============================================================================
// Assuming only one directional light for now
uniform DirLight u_DirLight;
uniform int u_HasDirLight; // Flag if directional light is active

uniform PointLight u_PointLights[MAX_POINT_LIGHTS]; // Array from lighting.glsl
uniform int u_NumPointLights; // Number of active point lights

uniform SpotLight u_SpotLights[MAX_SPOT_LIGHTS];   // Array from lighting.glsl
uniform int u_NumSpotLights;   // Number of active spot lights

// ============================================================================
// CAMERA UNIFORM
// ============================================================================
uniform vec3 u_ViewPos; // Camera position in world space

// ============================================================================
// HELPER: Get Normal from Map or Vertex Input
// ============================================================================
vec3 getShadingNormal() {
    vec3 N = normalize(fs_in.Normal); // Default to interpolated vertex normal
    if (u_HasNormalMap == 1) {
        // Sample tangent-space normal from map
        vec3 tangentNormal = texture(u_NormalMap, fs_in.TexCoords).xyz * 2.0 - 1.0;
        // Transform to world space using the TBN matrix
        N = normalize(fs_in.TBN * tangentNormal);
    }
    return N;
}

// ============================================================================
// MAIN FRAGMENT SHADER
// ============================================================================
void main() {
    // --- Sample Material Properties ---
    vec3 albedo = u_AlbedoColor;
    if (u_HasAlbedoMap == 1) {
        albedo = texture(u_AlbedoMap, fs_in.TexCoords).rgb;
    }

    float metallic = u_Metallic;
    if (u_HasMetallicMap == 1) {
        metallic = texture(u_MetallicMap, fs_in.TexCoords).r;
    }

    float roughness = u_Roughness;
    if (u_HasRoughnessMap == 1) {
        roughness = texture(u_RoughnessMap, fs_in.TexCoords).r;
    }

    float ao = u_AO;
    if (u_HasAOMap == 1) {
        ao = texture(u_AOMap, fs_in.TexCoords).r;
    }

    vec3 emissive = vec3(0.0);
    if (u_HasEmissiveMap == 1) {
        emissive = texture(u_EmissiveMap, fs_in.TexCoords).rgb * u_EmissiveColor; // Modulate by color if needed
    } else {
        emissive = u_EmissiveColor; // Use uniform color if no map
    }

    // --- Prepare PBR Inputs ---
    vec3 N = getShadingNormal();
    vec3 V = normalize(u_ViewPos - fs_in.FragPos); // View direction

    // Calculate F0 for Fresnel (reflectance at normal incidence)
    vec3 F0 = vec3(0.04); // Base reflectance for non-metals
    F0 = mix(F0, albedo, metallic); // Metals use albedo as F0

    // --- Initialize Lighting Accumulator ---
    vec3 Lo = vec3(0.0); // Outgoing radiance

    // ================== DIRECTIONAL LIGHT ==================
    if (u_HasDirLight == 1) {
        vec3 L = normalize(-u_DirLight.direction); // Direction TO light source
        vec3 H = normalize(V + L); // Halfway vector
        vec3 radiance = u_DirLight.color * u_DirLight.intensity;
        float NdotL = max(dot(N, L), 0.0);

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F; // Specular component (Fresnel)
        vec3 kD = vec3(1.0) - kS; // Diffuse component
        kD *= (1.0 - metallic); // Metals have no diffuse reflection

        // Specular BRDF term
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + EPSILON; // Add epsilon
        vec3 specular = numerator / denominator;

        // Calculate shadow attenuation
        float shadow = CalculateShadow(shadowMap, fs_in.FragPosLightSpace, N, L);

        // Add contribution to outgoing radiance (scaled by NdotL and shadow)
        Lo += (kD * albedo / PI + specular) * radiance * NdotL * shadow;
    }

    // ================== POINT LIGHTS ==================
    for (int i = 0; i < u_NumPointLights; ++i) {
        vec3 lightPos = u_PointLights[i].position;
        vec3 lightColor = u_PointLights[i].color;
        float intensity = u_PointLights[i].intensity;
        float constant = u_PointLights[i].constant;
        float linear = u_PointLights[i].linear;
        float quadratic = u_PointLights[i].quadratic;

        // Calculate per-light vectors
        vec3 L = normalize(lightPos - fs_in.FragPos); // Direction TO light source
        vec3 H = normalize(V + L); // Halfway vector
        float distance = length(lightPos - fs_in.FragPos);
        float NdotL = max(dot(N, L), 0.0);

        // Attenuation
        float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
        vec3 radiance = lightColor * intensity * attenuation;

        // Cook-Torrance BRDF (same as directional)
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= (1.0 - metallic);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + EPSILON;
        vec3 specular = numerator / denominator;

        // Add contribution (no shadows for point lights in this example)
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // ================== SPOT LIGHTS ==================
    for (int i = 0; i < u_NumSpotLights; ++i) {
        vec3 lightPos = u_SpotLights[i].position;
        vec3 lightDir = normalize(-u_SpotLights[i].direction); // Direction light is POINTING
        vec3 lightColor = u_SpotLights[i].color;
        float intensity = u_SpotLights[i].intensity;
        float cutOff = u_SpotLights[i].cutOff; // Cosine of inner angle
        float outerCutOff = u_SpotLights[i].outerCutOff; // Cosine of outer angle
        float constant = u_SpotLights[i].constant;
        float linear = u_SpotLights[i].linear;
        float quadratic = u_SpotLights[i].quadratic;

        // Calculate per-light vectors
        vec3 L = normalize(lightPos - fs_in.FragPos); // Direction TO light source
        vec3 H = normalize(V + L); // Halfway vector
        float distance = length(lightPos - fs_in.FragPos);
        float NdotL = max(dot(N, L), 0.0);

        // Spotlight intensity factor
        float theta = dot(L, lightDir); // Cosine between light direction and vector to fragment
        float epsilon = cutOff - outerCutOff;
        float spotFactor = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

        // Attenuation
        float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
        vec3 radiance = lightColor * intensity * attenuation * spotFactor;

        // Cook-Torrance BRDF (same as directional)
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= (1.0 - metallic);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + EPSILON;
        vec3 specular = numerator / denominator;

        // Add contribution (no shadows for spot lights in this example)
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

	
    // --- Ambient Light ---
    // Simple ambient term using AO factor (could be replaced by IBL later)
    vec3 ambient = vec3(0.03) * albedo * ao; // Basic ambient occlusion

    // --- Final Color ---
    vec3 color = ambient + Lo + emissive; // Add ambient, direct lighting, and emission

    // --- HDR Tonemapping & Gamma Correction (Placeholder) ---
    // These should ideally be done in a post-processing step
    // color = color / (color + vec3(1.0)); // Reinhard Tonemapping
    // color = pow(color, vec3(1.0/2.2));   // Gamma Correction

    FragColor = vec4(color, 1.0); // Assuming opaque for now
	
}