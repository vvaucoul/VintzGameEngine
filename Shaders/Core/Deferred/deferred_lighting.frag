#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

// G-Buffer samplers
uniform sampler2D gAlbedoAO;         // Albedo Color (rgb), AO (a)
uniform sampler2D gPositionMetallic; // World Pos (xyz), Metallic (w)
uniform sampler2D gNormalRoughness;  // World Normal (xyz), Roughness (w)
// uniform sampler2D gEmissiveSpecular; // Optional

// Other uniforms
uniform sampler2D shadowMap;
uniform vec3 u_ViewPos; // Camera position in world space
uniform mat4 lightSpaceMatrix;

// --- PBR Cook-Torrance BRDF functions (same as in pbr.frag) ---
const float PI = 3.14159265359;

// Normal Distribution Function (NDF) - Trowbridge-Reitz GGX
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
    float k = (r * r) / 8.0; // k for direct lighting
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}
// Geometry Function - Smith's method
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

// Fresnel Equation - Schlick approximation
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// --- End PBR Functions ---


// --- Shadow Calculation ---
float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Check if outside shadow map bounds
    if (projCoords.z > 1.0) return 0.0;

    // Get closest depth from shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // Get current depth
    float currentDepth = projCoords.z;

    // Calculate bias (slope-based)
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);

    // PCF - Percentage-Closer Filtering
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}


// --- Light Structs (match C++ side, e.g., UBO layout) ---
struct PointLight {
    vec3 position;
    float constant;
    vec3 color;
    float linear;
    // No quadratic needed if passed via uniform
    float quadratic;
    // Add padding if needed for std140 layout
};

struct DirectionalLight {
    vec3 direction;
    // padding
    vec3 color;
    // padding
};

// Assume lights are passed via UBO or arrays (example with arrays)
#define MAX_POINT_LIGHTS 10
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_NumPointLights;

#define MAX_DIR_LIGHTS 1
uniform DirectionalLight u_DirLights[MAX_DIR_LIGHTS];
uniform int u_NumDirLights;


void main() {
    // --- Sample G-Buffer ---
    vec3 FragPos   = texture(gPositionMetallic, TexCoords).xyz;
    float metallic = texture(gPositionMetallic, TexCoords).w;
    vec3 Normal    = texture(gNormalRoughness, TexCoords).xyz;
    float roughness= texture(gNormalRoughness, TexCoords).w;
    vec3 Albedo    = texture(gAlbedoAO, TexCoords).rgb;
    float ao       = texture(gAlbedoAO, TexCoords).a;

 	// --- DEBUG: Output Albedo directly ---
    // FragColor = vec4(Albedo, 1.0); // Output Albedo directly
	// FragColor = vec4(0.0, 1.0, 0.0, 1.0); // Output green for debugging
	// return; // Skip the rest of the shader for debugging

    // --- Prepare PBR inputs ---
    vec3 N = normalize(Normal);
    vec3 V = normalize(u_ViewPos - FragPos); // View direction
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, metallic);

    // --- Lighting Calculation ---
    vec3 Lo = vec3(0.0);

    // --- Directional Lights ---
    for (int i = 0; i < u_NumDirLights; ++i) {
        vec3 lightDir = normalize(-u_DirLights[i].direction); // Direction TO light source
        vec3 lightColor = u_DirLights[i].color;

        // Calculate PBR terms
        vec3 H = normalize(V + lightDir); // Halfway vector
        float NdotL = max(dot(N, lightDir), 0.0);

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, lightDir, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F; // Specular component (Fresnel)
        vec3 kD = vec3(1.0) - kS; // Diffuse component
        kD *= (1.0 - metallic); // Metals have no diffuse light

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001; // Add epsilon
        vec3 specular = numerator / denominator;

        // Shadow calculation
        vec4 fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
        float shadow = CalculateShadow(fragPosLightSpace, N, lightDir);

        // Add to outgoing radiance (scaled by color, NdotL, and shadow)
        Lo += (kD * Albedo / PI + specular) * lightColor * NdotL * (1.0 - shadow);
    }

    // --- Point Lights ---
    for (int i = 0; i < u_NumPointLights; ++i) {
        vec3 lightPos = u_PointLights[i].position;
        vec3 lightColor = u_PointLights[i].color;
        float constant = u_PointLights[i].constant;
        float linear = u_PointLights[i].linear;
        float quadratic = u_PointLights[i].quadratic;

        // Calculate per-light vectors
        vec3 lightDir = normalize(lightPos - FragPos); // Direction TO light source
        vec3 H = normalize(V + lightDir); // Halfway vector
        float distance = length(lightPos - FragPos);
        float NdotL = max(dot(N, lightDir), 0.0);

        // Attenuation
        float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

        // Cook-Torrance BRDF (same as directional)
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, lightDir, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= (1.0 - metallic);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
        vec3 specular = numerator / denominator;

        // Add to outgoing radiance (scaled by color, NdotL, and attenuation)
        // Note: Point light shadows are more complex and not included here
        Lo += (kD * Albedo / PI + specular) * lightColor * NdotL * attenuation;
    }


    // --- Ambient Light ---
    // Simple ambient term using AO factor
    vec3 ambient = vec3(0.03) * Albedo * ao;

    // --- Final Color ---
    vec3 color = ambient + Lo;
    FragColor = vec4(color, 1.0); // Commented out for debugging

    // Add emissive if used
    // color += Emissive;

    // HDR Tonemapping (simple Reinhard) - Should ideally be done in post-processing
    // color = color / (color + vec3(1.0));

    // Gamma correction (simple) - Also better in post-processing
    // color = pow(color, vec3(1.0/2.2));
}