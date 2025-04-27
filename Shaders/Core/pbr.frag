#version 450 core

out vec4 FragColor;

// Input from vertex shader
in vec3 FragPos;           // World space position
in vec3 Normal;            // World space normal
in vec2 TexCoords;         // Texture coordinates
in mat3 TBN;               // Tangent-Bitangent-Normal matrix
in vec4 FragPosLightSpace; // Fragment position in light space

// --- Material Uniforms ---
uniform sampler2D u_AlbedoMap;    // Texture unit 0
uniform sampler2D u_NormalMap;    // Texture unit 1
// uniform sampler2D u_ORMMap;    // Texture unit 2 (Occlusion, Roughness, Metallic) - Not used currently
uniform sampler2D u_AOMap;        // Texture unit 3 (Separate AO)
uniform sampler2D shadowMap;      // Texture unit 4 (Shadow Map)

uniform int u_HasAlbedoMap;
uniform int u_HasNormalMap;
// uniform int u_HasORMMap; // Not used currently
uniform int u_HasAOMap;

uniform vec3 u_AlbedoColor; // Fallback color if no albedo map
uniform float u_Metallic;   // Fallback metallic if no ORM map
uniform float u_Roughness;  // Fallback roughness if no ORM map
uniform float u_AO;         // Fallback AO if no AO/ORM map

// --- Light Structs ---
struct DirLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float cutOff;       // Cosine of inner angle
    float outerCutOff;  // Cosine of outer angle
    float constant;
    float linear;
    float quadratic;
};

// --- Light Uniforms ---
uniform DirLight dirLight;

#define MAX_POINT_LIGHTS 4 // Match C++ side
uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

#define MAX_SPOT_LIGHTS 4 // Match C++ side (or adjust as needed)
uniform int numSpotLights;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

// --- Camera & Shadow Uniforms ---
uniform vec3 u_ViewPos;        // Camera position in world space
uniform mat4 lightSpaceMatrix; // For shadow calculation

const float PI = 3.14159265359;

// --- PBR Helper Functions ---

// Normal Distribution Function (NDF) - GGX/Trowbridge-Reitz
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / max(denom, 0.0000001); // Prevent divide by zero
}

// Geometry Function - Schlick-GGX
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0; // Direct light
    // float k = (roughness * roughness) / 2.0; // IBL
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / max(denom, 0.0000001); // Prevent divide by zero
}

// Geometry Function - Smith's method
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

// Fresnel Function - Schlick approximation
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// --- Shadow Calculation ---
float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Check if fragment is outside the light's frustum
    if (projCoords.z > 1.0) return 0.0;

    // Get closest depth value from light's perspective (using PCF)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005); // Shadow bias

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (projCoords.z - bias) > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return 1.0 - shadow;
}

// --- Lighting Calculation Functions ---

// Calculates the PBR contribution from a directional light
vec3 CalcDirLightPBR(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness, float ao) {
    vec3 L = normalize(-light.direction);
    vec3 H = normalize(viewDir + L);
    float NdotL = max(dot(normal, L), 0.0);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, H, roughness);
    float G = GeometrySmith(normal, viewDir, L, roughness);
    vec3 F0 = mix(vec3(0.04), albedo, metallic); // Base reflectivity
    vec3 F = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= (1.0 - metallic); // Non-metals have diffuse component

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * NdotL + 0.0001; // Prevent divide by zero
    vec3 specular = numerator / denominator;

    // Add shadow calculation
    float shadow = CalculateShadow(FragPosLightSpace, normal, L);

    // Radiance
    vec3 Lo = light.color * light.intensity * NdotL;

    // Combine and apply AO + Shadow
    return (kD * albedo / PI + specular) * Lo * ao * shadow;
}

// Calculates the PBR contribution from a point light
vec3 CalcPointLightPBR(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float metallic, float roughness, float ao) {
    vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(viewDir + L);
    float NdotL = max(dot(normal, L), 0.0);

    // Cook-Torrance BRDF (same as directional)
    float NDF = DistributionGGX(normal, H, roughness);
    float G = GeometrySmith(normal, viewDir, L, roughness);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= (1.0 - metallic);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * NdotL + 0.0001;
    vec3 specular = numerator / denominator;

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Radiance
    vec3 Lo = light.color * light.intensity * NdotL * attenuation;

    // Combine and apply AO
    return (kD * albedo / PI + specular) * Lo * ao;
    // Note: Point light shadows are more complex (omnidirectional) and not implemented here.
}

// Implement CalcSpotLightPBR function
vec3 CalcSpotLightPBR(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float metallic, float roughness, float ao) {
    vec3 L = normalize(light.position - fragPos);
    float theta = dot(L, normalize(-light.direction)); // Angle between light direction and fragment direction

    // Check if fragment is inside the spotlight cone
    if (theta > light.outerCutOff) { // Use pre-calculated cosine values
        vec3 H = normalize(viewDir + L);
        float NdotL = max(dot(normal, L), 0.0);

        // Cook-Torrance BRDF (same as point light)
        float NDF = DistributionGGX(normal, H, roughness);
        float G = GeometrySmith(normal, viewDir, L, roughness);
        vec3 F0 = mix(vec3(0.04), albedo, metallic);
        vec3 F = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= (1.0 - metallic);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * NdotL + 0.0001;
        vec3 specular = numerator / denominator;

        // Attenuation (same as point light)
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        // Spotlight intensity falloff (smooth edge)
        float epsilon = light.cutOff - light.outerCutOff;
        float intensityFactor = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

        // Radiance
        vec3 Lo = light.color * light.intensity * NdotL * attenuation * intensityFactor;

        // Combine and apply AO
        return (kD * albedo / PI + specular) * Lo * ao;
        // Note: Spot light shadows are also complex and not implemented here.
    } else {
        // Outside the spotlight cone
        return vec3(0.0);
    }
}

void main() {
    // --- Get Material Properties ---
    vec3 albedo = u_AlbedoColor;
    if (u_HasAlbedoMap == 1) {
        albedo = texture(u_AlbedoMap, TexCoords).rgb;
        // Basic gamma correction if albedo texture is sRGB
        albedo = pow(albedo, vec3(2.2));
    }

    float metallic = u_Metallic;
    float roughness = u_Roughness;
    float ao = u_AO;

    // If ORM map was used, sample it here:
    // if (u_HasORMMap == 1) {
    //     vec3 orm = texture(u_ORMMap, TexCoords).rgb;
    //     ao = orm.r;
    //     roughness = orm.g;
    //     metallic = orm.b;
    // }
    // If separate AO map is used:
    if (u_HasAOMap == 1) {
        ao = texture(u_AOMap, TexCoords).r;
    }

    // --- Get Normal ---
    vec3 N = normalize(Normal); // Use interpolated normal by default
    if (u_HasNormalMap == 1) {
        // Sample normal map, transform from tangent to world space
        vec3 tangentNormal = texture(u_NormalMap, TexCoords).rgb * 2.0 - 1.0;
        N = normalize(TBN * tangentNormal);
    }

    // --- View Direction ---
    vec3 V = normalize(u_ViewPos - FragPos);

    // --- Calculate Lighting ---
    // Base reflectivity for non-metals
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // Initialize lighting result (Lo)
    vec3 Lo = vec3(0.0);

    // Add directional light contribution
    Lo += CalcDirLightPBR(dirLight, N, V, albedo, metallic, roughness, ao);

    // Add point light contributions
    for (int i = 0; i < numPointLights; ++i) {
        Lo += CalcPointLightPBR(pointLights[i], N, FragPos, V, albedo, metallic, roughness, ao);
    }

    // Add spot light contributions
    for(int i = 0; i < numSpotLights; ++i) {
        Lo += CalcSpotLightPBR(spotLights[i], N, FragPos, V, albedo, metallic, roughness, ao);
    }

    // Ambient term (simple ambient occlusion)
    // A more advanced approach would use IBL (Image-Based Lighting)
    vec3 ambient = vec3(0.03 * ao); // Basic ambient light scaled by AO
    vec3 color = ambient * albedo + Lo;

    // HDR Tonemapping and Gamma Correction will be handled in the final post-processing pass
    // For now, output the HDR color directly
    FragColor = vec4(color, 1.0);

    // --- DEBUG OUTPUTS ---
    // FragColor = vec4(albedo, 1.0);
    // FragColor = vec4(N * 0.5 + 0.5, 1.0); // Visualize normals
    // FragColor = vec4(metallic, metallic, metallic, 1.0);
    // FragColor = vec4(roughness, roughness, roughness, 1.0);
    // FragColor = vec4(ao, ao, ao, 1.0);
    // float shadow = CalculateShadow(FragPosLightSpace, N, normalize(-dirLight.direction)); FragColor = vec4(vec3(shadow), 1.0); // Visualize shadow
}
