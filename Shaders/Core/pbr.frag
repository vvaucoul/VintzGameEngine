#version 450 core

// ============================================================================
// OUTPUT
// ============================================================================
out vec4 FragColor;

// ============================================================================
// INPUTS FROM VERTEX SHADER
// ============================================================================
in vec3 FragPos;           // World space position
in vec3 Normal;            // World space normal (geometric)
in vec2 TexCoords;         // Texture coordinates
in mat3 TBN;               // Tangent-Bitangent-Normal matrix (World -> Tangent)
in vec4 FragPosLightSpace; // Fragment position in light space (for shadows)
// Note: For Anisotropy, we might need Tangent and Bitangent explicitly
in vec3 WorldTangent;      // World space tangent vector (derived from TBN in VS)
in vec3 WorldBitangent;    // World space bitangent vector (derived from TBN in VS)


// ============================================================================
// TEXTURE SAMPLERS (Assign appropriate texture units in C++)
// ============================================================================
// Base PBR
uniform sampler2D u_AlbedoMap;     // Unit 0
uniform sampler2D u_NormalMap;     // Unit 1
uniform sampler2D u_MetallicMap;   // Unit 2 (Assuming separate maps now)
uniform sampler2D u_RoughnessMap;  // Unit 3
uniform sampler2D u_AOMap;         // Unit 4
// Extended Features
uniform sampler2D u_EmissiveMap;   // Unit 5
uniform sampler2D u_OpacityMap;    // Unit 6
uniform sampler2D u_HeightMap;     // Unit 7
uniform sampler2D u_ClearcoatMap;  // Unit 8 (Can pack intensity/roughness)
uniform sampler2D u_AnisotropyMap; // Unit 9 (Can pack strength/direction)
uniform sampler2D u_SubsurfaceMap; // Unit 10 (Can pack thickness/color)
// Other
uniform sampler2D shadowMap;       // Unit 11 (Directional light shadow)

// ============================================================================
// MATERIAL UNIFORMS (Match MaterialPBR struct)
// ============================================================================
// Base PBR
uniform vec3 u_AlbedoColor;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_AO;
// Emission
uniform vec3 u_EmissiveColor;
// Opacity
uniform float u_Opacity;
// Parallax
uniform float u_ParallaxScale;
// Clearcoat
uniform float u_Clearcoat;
uniform float u_ClearcoatRoughness;
// Sheen
uniform vec3 u_SheenColor;
uniform float u_SheenRoughness;
// Anisotropy
uniform float u_Anisotropy;         // Strength (0 = isotropic, 1 = max anisotropic)
uniform vec3 u_AnisotropyDirection; // Base direction in tangent space (often {1,0,0})
// Subsurface
uniform float u_Subsurface;         // Strength/Amount
uniform vec3 u_SubsurfaceColor;    // Color tint for scattering

// --- Map Presence Flags ---
uniform int u_HasAlbedoMap;
uniform int u_HasNormalMap;
uniform int u_HasMetallicMap;
uniform int u_HasRoughnessMap;
uniform int u_HasAOMap;
uniform int u_HasEmissiveMap;
uniform int u_HasOpacityMap;
uniform int u_HasHeightMap;
uniform int u_HasClearcoatMap;
uniform int u_HasAnisotropyMap;
uniform int u_HasSubsurfaceMap;

// --- Other Properties ---
// uniform int u_DoubleSided; // Handled by glDisable(GL_CULL_FACE) typically

// ============================================================================
// LIGHT STRUCTS & UNIFORMS (Keep as is)
// ============================================================================
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
    float cutOff;      // Cosine of inner angle
    float outerCutOff; // Cosine of outer angle
    float constant;
    float linear;
    float quadratic;
};

uniform DirLight dirLight;

#define MAX_POINT_LIGHTS 4
uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

#define MAX_SPOT_LIGHTS 4
uniform int numSpotLights;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

// ============================================================================
// CAMERA & SHADOW UNIFORMS (Keep as is)
// ============================================================================
uniform vec3 u_ViewPos;        // Camera position in world space
uniform mat4 lightSpaceMatrix; // Light's view-projection matrix

// ============================================================================
// CONSTANTS
// ============================================================================
const float PI = 3.14159265359;
const float EPSILON = 1e-5; // Small value to prevent division by zero

// ============================================================================
// PBR HELPER FUNCTIONS (Base Isotropic)
// ============================================================================

// GGX/Trowbridge-Reitz Normal Distribution Function (NDF) - Isotropic
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
    return nom / max(denom, EPSILON);
}

// Schlick-GGX Geometry Function (for direct lighting)
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / max(denom, EPSILON);
}

// Smith's Geometry Function (combines view and light) - Isotropic
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggxV = GeometrySchlickGGX(NdotV, roughness);
    float ggxL = GeometrySchlickGGX(NdotL, roughness);
    return ggxV * ggxL;
}

// Fresnel Schlick approximation
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Fresnel for Rough Surfaces (approximated, useful for clearcoat attenuation)
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// ============================================================================
// PBR HELPER FUNCTIONS (Anisotropic GGX)
// ============================================================================

// Anisotropic GGX NDF (Burley's derivation)
// T: Tangent direction, B: Bitangent direction
// alpha_x: roughness along T, alpha_y: roughness along B
float DistributionGGX_Aniso(vec3 N, vec3 H, vec3 T, vec3 B, float alpha_x, float alpha_y) {
    float dotHT = dot(H, T);
    float dotHB = dot(H, B);
    float dotHN = max(dot(H, N), 0.0);

    float a = dotHT / alpha_x;
    float b = dotHB / alpha_y;
    float c = a*a + b*b + dotHN*dotHN; // Denominator term before sqrt and power

    if (c == 0.0) return 0.0; // Avoid division by zero if H is orthogonal to T, B, N

    float denom = PI * alpha_x * alpha_y * c * c; // Note: c is squared twice

    return 1.0 / max(denom, EPSILON);
}


// Smith's Geometry Function for Anisotropic GGX (Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs")
// V: View or Light vector
float GeometrySmith_Aniso(vec3 N, vec3 V, vec3 T, vec3 B, float alpha_x, float alpha_y) {
    float dotVT = dot(V, T);
    float dotVB = dot(V, B);
    float dotVN = max(dot(V, N), 0.0);

    if (dotVN == 0.0) return 0.0; // Avoid division by zero

    // Calculate projected area roughness
    float lambda_denom = (dotVT*dotVT) * (alpha_x*alpha_x) + (dotVB*dotVB) * (alpha_y*alpha_y);
    if (lambda_denom == 0.0) return 1.0; // Avoid division by zero if V is aligned with N

    float lambda = dotVN / sqrt(lambda_denom);

    // Approximation of the integral for Smith G1
    float G1 = 2.0 / (1.0 + sqrt(1.0 + 1.0 / (lambda * lambda)));

    return G1;
}

// Combined Smith Geometry for Anisotropic GGX
float GeometrySmithCombined_Aniso(vec3 N, vec3 V, vec3 L, vec3 T, vec3 B, float alpha_x, float alpha_y) {
    float G_V = GeometrySmith_Aniso(N, V, T, B, alpha_x, alpha_y);
    float G_L = GeometrySmith_Aniso(N, L, T, B, alpha_x, alpha_y);
    return G_V * G_L;
}


// ============================================================================
// PARALLAX MAPPING
// ============================================================================
vec2 parallaxMapping(vec2 texCoords, vec3 viewDirTangent) {
    if (u_HasHeightMap == 0 || u_ParallaxScale <= 0.0) {
        return texCoords;
    }

    // Number of layers for steep parallax
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDirTangent)));

    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    // The direction to step in texture space
    vec2 P = viewDirTangent.xy / viewDirTangent.z * u_ParallaxScale;
    vec2 deltaTexCoords = P / numLayers;

    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(u_HeightMap, currentTexCoords).r;

    // Step through layers until view ray penetrates the surface
    while(currentLayerDepth < currentDepthMapValue) {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(u_HeightMap, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }

    // --- Parallax Occlusion Mapping Refinement ---
    // Step back one layer
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // Depth before and after intersection.
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(u_HeightMap, prevTexCoords).r - currentLayerDepth + layerDepth;

    // Interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    // Boundary check (discard or clamp)
    if(finalTexCoords.x < 0.0 || finalTexCoords.x > 1.0 || finalTexCoords.y < 0.0 || finalTexCoords.y > 1.0) {
       // return texCoords; // Fallback to original coords if out of bounds
       discard; // Or discard if artifacts are preferred over stretching
    }

    return finalTexCoords;
}

// ============================================================================
// SHADOW CALCULATION (Directional Light, PCF - Keep as is)
// ============================================================================
float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) return 1.0; // No shadow if outside frustum (was 0.0, should be 1.0 for "no shadow")

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (projCoords.z - bias) > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return 1.0 - shadow; // Return amount of light (1.0 = fully lit, 0.0 = shadowed)
}

// ============================================================================
// LIGHTING CALCULATION FUNCTIONS (Modified for new features)
// ============================================================================

struct PBRSurface {
    vec3 Albedo;
    float Metallic;
    float Roughness; // Base roughness
    float AO;
    vec3 F0;         // Base Fresnel reflectance at 0 incidence
    vec3 N;          // Shading normal
    vec2 TexCoords;  // Final texture coordinates after parallax
    // Anisotropy related
    float Anisotropy;
    float AnisoRoughnessX; // Effective roughness along Tangent
    float AnisoRoughnessY; // Effective roughness along Bitangent
    vec3 Tangent;         // World space tangent
    vec3 Bitangent;       // World space bitangent
    // Clearcoat related
    float Clearcoat;
    float ClearcoatRoughness;
    vec3 F_Clearcoat; // Fresnel term for clearcoat layer
    // Sheen related
    vec3 SheenColor;
    float SheenRoughness;
    // SSS related
    float Subsurface;
    vec3 SubsurfaceColor;
};

// Calculate Direct Lighting Contribution (for one light)
vec3 CalcDirectLight(vec3 L, vec3 V, vec3 N, vec3 radiance, PBRSurface surface) {
    vec3 H = normalize(V + L);
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float HdotV = max(dot(H, V), 0.0);

    if (NdotL <= 0.0) return vec3(0.0); // Light is behind the surface

    // --- Base Layer BRDF ---
    vec3 baseSpecular = vec3(0.0);
    vec3 baseDiffuse = vec3(0.0);
    vec3 F_base = fresnelSchlick(HdotV, surface.F0); // Fresnel for base layer

    if (surface.Anisotropy > 0.0) {
        // Anisotropic BRDF
        float NDF = DistributionGGX_Aniso(N, H, surface.Tangent, surface.Bitangent, surface.AnisoRoughnessX, surface.AnisoRoughnessY);
        float G   = GeometrySmithCombined_Aniso(N, V, L, surface.Tangent, surface.Bitangent, surface.AnisoRoughnessX, surface.AnisoRoughnessY);
        vec3 numerator = NDF * G * F_base;
        float denominator = 4.0 * NdotV * NdotL + EPSILON;
        baseSpecular = numerator / denominator;
    } else {
        // Isotropic BRDF
        float NDF = DistributionGGX(N, H, surface.Roughness);
        float G   = GeometrySmith(N, V, L, surface.Roughness);
        vec3 numerator = NDF * G * F_base;
        float denominator = 4.0 * NdotV * NdotL + EPSILON;
        baseSpecular = numerator / denominator;
    }

    vec3 kS = F_base;
    vec3 kD = (1.0 - kS) * (1.0 - surface.Metallic); // Diffuse factor
    baseDiffuse = kD * surface.Albedo / PI;

    // --- Clearcoat Layer BRDF ---
    vec3 clearcoatSpecular = vec3(0.0);
    if (surface.Clearcoat > 0.0) {
        float F0_cc = 0.04; // Fresnel for dielectric clearcoat
        vec3 F_cc = fresnelSchlick(HdotV, vec3(F0_cc));
        float NDF_cc = DistributionGGX(N, H, surface.ClearcoatRoughness);
        float G_cc   = GeometrySmith(N, V, L, surface.ClearcoatRoughness);

        vec3 numerator_cc = NDF_cc * G_cc * F_cc;
        float denominator_cc = 4.0 * NdotV * NdotL + EPSILON;
        clearcoatSpecular = (numerator_cc / denominator_cc) * surface.Clearcoat; // Modulate by clearcoat intensity

        // Attenuate base layer by clearcoat reflection (energy conservation)
        // Use Fresnel term based on NdotV for transmission through clearcoat layer
        vec3 F_transmission = vec3(1.0) - fresnelSchlickRoughness(NdotV, vec3(F0_cc), surface.ClearcoatRoughness) * surface.Clearcoat;
        baseDiffuse *= F_transmission;
        baseSpecular *= F_transmission; // Should also attenuate specular
    }

    // --- Sheen Layer ---
    vec3 sheen = vec3(0.0);
    if (dot(surface.SheenColor, surface.SheenColor) > 0.0) { // Check if sheen color is non-black
         // Simple model: More sheen at grazing angles (low HdotV)
         float sheenFactor = pow(1.0 - HdotV, 5.0); // Similar to Fresnel term
         // Mix sheen roughness into factor (higher roughness = wider sheen)
         sheenFactor *= (1.0 - surface.SheenRoughness * 0.5); // Adjust multiplier as needed
         // Use Fresnel-like term for sheen color mixing
         vec3 F_sheen = fresnelSchlick(HdotV, surface.SheenColor); // Use sheen color as F0
         sheen = F_sheen * sheenFactor * (1.0 - surface.Metallic); // Sheen typically on non-metals
    }

    // --- Subsurface Scattering (Simple Transmission/Wrap Approximation) ---
    vec3 sss = vec3(0.0);
    if (surface.Subsurface > 0.0) {
        // Simple wrap lighting: light contribution increases as L points away from V
        float wrap = 0.5; // Controls how much light wraps around (0 to 1)
        float NdotL_wrapped = max(0.0, (dot(N, L) + wrap) / (1.0 + wrap));
        // Attenuate based on view angle (more scattering at grazing angles)
        float scatterFactor = pow(1.0 - NdotV, 2.0); // Adjust power as needed
        // Combine with subsurface color and amount
        sss = surface.SubsurfaceColor * surface.Albedo * NdotL_wrapped * surface.Subsurface * scatterFactor * (1.0 - surface.Metallic);
        // Reduce direct diffuse contribution to conserve energy (approximate)
        baseDiffuse *= (1.0 - surface.Subsurface * 0.5);
    }


    // Combine all components
    // Note: Sheen and SSS are often added to diffuse or handled separately depending on the model
    vec3 totalDiffuse = baseDiffuse + sheen + sss;
    vec3 totalSpecular = baseSpecular + clearcoatSpecular;

    return (totalDiffuse + totalSpecular) * radiance * NdotL;
}


// ============================================================================
// MAIN FRAGMENT SHADER ENTRY POINT
// ============================================================================
void main() {
    // ------------------------------------------------------------------------
    // 0. VIEW DIRECTION & TANGENT SPACE VIEW DIR
    // ------------------------------------------------------------------------
    vec3 V = normalize(u_ViewPos - FragPos);
    // Need view direction in Tangent Space for Parallax
    // TBN is World -> Tangent. Use transpose(TBN) which is Tangent -> World.
    // Or directly use TBN if it's Tangent -> World. Assuming World -> Tangent here.
    mat3 TBN_inv = transpose(TBN); // Tangent -> World
    vec3 viewDirTangent = normalize(TBN * V); // Transform World View Dir to Tangent Space

    // ------------------------------------------------------------------------
    // 1. PARALLAX MAPPING
    // ------------------------------------------------------------------------
    vec2 finalTexCoords = parallaxMapping(TexCoords, viewDirTangent);

    // ------------------------------------------------------------------------
    // 2. MATERIAL PROPERTY FETCH
    // ------------------------------------------------------------------------
    PBRSurface surface;
    surface.TexCoords = finalTexCoords;

    // Albedo & Opacity
    vec4 albedoSample = vec4(u_AlbedoColor, u_Opacity);
    if (u_HasAlbedoMap == 1) {
        albedoSample = texture(u_AlbedoMap, surface.TexCoords);
        // Gamma correct albedo if texture is sRGB (Consider doing this at the end)
        // albedoSample.rgb = pow(albedoSample.rgb, vec3(2.2));
    }
	
    surface.Albedo = albedoSample.rgb;
    float alpha = albedoSample.a * u_Opacity; // Combine texture alpha and uniform opacity

    // Opacity Map Override
    if (u_HasOpacityMap == 1) {
        alpha *= texture(u_OpacityMap, surface.TexCoords).r; // Assuming grayscale opacity map
    }

    // --- Early discard based on final alpha ---
    // if (alpha < 0.01) { // Use an appropriate threshold
    //     discard;
    // }

    // Metallic
    surface.Metallic = u_Metallic;
    if (u_HasMetallicMap == 1) {
        surface.Metallic *= texture(u_MetallicMap, surface.TexCoords).r; // Assume R channel
    }

    // Roughness (Base)
    surface.Roughness = u_Roughness;
    if (u_HasRoughnessMap == 1) {
        surface.Roughness *= texture(u_RoughnessMap, surface.TexCoords).r; // Assume R channel
    }
    surface.Roughness = clamp(surface.Roughness, 0.01, 1.0); // Clamp to avoid pure black specular

    // Ambient Occlusion
    surface.AO = u_AO;
    if (u_HasAOMap == 1) {
        surface.AO *= texture(u_AOMap, surface.TexCoords).r; // Assume R channel
    }

    // Emission
    vec3 emission = u_EmissiveColor;
    if (u_HasEmissiveMap == 1) {
        emission *= texture(u_EmissiveMap, surface.TexCoords).rgb;
    }

    // Clearcoat
    surface.Clearcoat = u_Clearcoat;
    surface.ClearcoatRoughness = u_ClearcoatRoughness;
    if (u_HasClearcoatMap == 1) {
        vec2 ccSample = texture(u_ClearcoatMap, surface.TexCoords).rg; // Assume R=Intensity, G=Roughness
        surface.Clearcoat *= ccSample.r;
        surface.ClearcoatRoughness = mix(surface.ClearcoatRoughness, ccSample.g, surface.Clearcoat); // Blend roughness based on intensity
    }
    surface.ClearcoatRoughness = clamp(surface.ClearcoatRoughness, 0.01, 1.0);

    // Sheen
    surface.SheenColor = u_SheenColor;
    surface.SheenRoughness = u_SheenRoughness;
    // Add map sampling if needed (e.g., sheen intensity map)

    // Anisotropy
    surface.Anisotropy = u_Anisotropy;
    vec3 anisotropyDirection = u_AnisotropyDirection; // Tangent space default
    if (u_HasAnisotropyMap == 1) {
        vec3 anisoSample = texture(u_AnisotropyMap, surface.TexCoords).rgb; // R=Strength, GB=Direction offset/rotation? Depends on packing.
        surface.Anisotropy *= anisoSample.r;
        // Complex: Decode direction from map if needed
    }
    // Calculate anisotropic roughness values
    float aspect = sqrt(1.0 - surface.Anisotropy * 0.9); // From Filament docs, prevents zero roughness
    surface.AnisoRoughnessX = surface.Roughness / aspect;
    surface.AnisoRoughnessY = surface.Roughness * aspect;
    surface.AnisoRoughnessX = clamp(surface.AnisoRoughnessX, 0.01, 1.0);
    surface.AnisoRoughnessY = clamp(surface.AnisoRoughnessY, 0.01, 1.0);

    // Subsurface
    surface.Subsurface = u_Subsurface;
    surface.SubsurfaceColor = u_SubsurfaceColor;
    if (u_HasSubsurfaceMap == 1) {
         // Example: R=Thickness/Amount, GBA=Color Tint Multiplier
         vec4 sssSample = texture(u_SubsurfaceMap, surface.TexCoords);
         surface.Subsurface *= sssSample.r;
         surface.SubsurfaceColor *= sssSample.gba; // Modulate base color
    }

    // ------------------------------------------------------------------------
    // 3. NORMAL CALCULATION
    // ------------------------------------------------------------------------
    vec3 N_geom = normalize(Normal); // Geometric normal
    surface.N = N_geom;
    if (u_HasNormalMap == 1) {
        vec3 tangentNormal = texture(u_NormalMap, surface.TexCoords).rgb * 2.0 - 1.0;
        // Transform tangent-space normal to world space using transpose of TBN
        // Assumes TBN is World -> Tangent
        surface.N = normalize(TBN_inv * tangentNormal);
    }
    // For double-sided materials without GL culling, check gl_FrontFacing
    // if (!gl_FrontFacing) surface.N = -surface.N;

    // ------------------------------------------------------------------------
    // 4. TANGENT / BITANGENT (World Space) & ANISOTROPY DIRECTION
    // ------------------------------------------------------------------------
    // Get World Tangent/Bitangent (passed from VS or derived from TBN_inv and N)
    surface.Tangent = normalize(WorldTangent); // Ensure normalized
    surface.Bitangent = normalize(WorldBitangent); // Ensure normalized
    // Refine Tangent/Bitangent based on Normal Map if needed (Gram-Schmidt)
    if (u_HasNormalMap == 1) {
         surface.Tangent = normalize(surface.Tangent - dot(surface.Tangent, surface.N) * surface.N);
         surface.Bitangent = cross(surface.N, surface.Tangent); // Recalculate bitangent
    }
    // Transform anisotropy direction from tangent to world if needed
    // vec3 worldAnisoDir = TBN_inv * anisotropyDirection;


    // ------------------------------------------------------------------------
    // 5. BASE FRESNEL (F0)
    // ------------------------------------------------------------------------
    surface.F0 = mix(vec3(0.04), surface.Albedo, surface.Metallic);

    // ------------------------------------------------------------------------
    // 6. LIGHTING ACCUMULATION
    // ------------------------------------------------------------------------
    vec3 Lo = vec3(0.0); // Accumulated outgoing radiance

    // Directional light
    if (dirLight.intensity > 0.0) {
        vec3 L = normalize(-dirLight.direction);
        vec3 radiance = dirLight.color * dirLight.intensity;
        float shadow = CalculateShadow(FragPosLightSpace, surface.N, L);
        Lo += CalcDirectLight(L, V, surface.N, radiance, surface) * shadow;
    }

    // Point lights
    for (int i = 0; i < numPointLights; ++i) {
        if (pointLights[i].intensity > 0.0) {
            vec3 L = normalize(pointLights[i].position - FragPos);
            float distance = length(pointLights[i].position - FragPos);
            float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));
            vec3 radiance = pointLights[i].color * pointLights[i].intensity * attenuation;
            // Point light shadows are complex, omitted here
            Lo += CalcDirectLight(L, V, surface.N, radiance, surface);
        }
    }

    // Spot lights
    for (int i = 0; i < numSpotLights; ++i) {
         if (spotLights[i].intensity > 0.0) {
            vec3 L = normalize(spotLights[i].position - FragPos);
            float theta = dot(L, normalize(-spotLights[i].direction));
            if (theta > spotLights[i].outerCutOff) { // Check cone
                float distance = length(spotLights[i].position - FragPos);
                float attenuation = 1.0 / (spotLights[i].constant + spotLights[i].linear * distance + spotLights[i].quadratic * (distance * distance));
                float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
                float intensityFactor = clamp((theta - spotLights[i].outerCutOff) / max(epsilon, EPSILON), 0.0, 1.0);
                vec3 radiance = spotLights[i].color * spotLights[i].intensity * attenuation * intensityFactor;
                // Spot light shadows are complex, omitted here
                Lo += CalcDirectLight(L, V, surface.N, radiance, surface);
            }
        }
    }

    // ------------------------------------------------------------------------
    // 7. AMBIENT TERM (AO ONLY - Replace with IBL for better results)
    // ------------------------------------------------------------------------
    // Very basic ambient, strongly recommend Image-Based Lighting (IBL) instead
    vec3 ambient = vec3(0.03) * surface.Albedo * surface.AO;

    // ------------------------------------------------------------------------
    // 8. COMBINE FINAL COLOR & EMISSION
    // ------------------------------------------------------------------------
    vec3 color = ambient + Lo + emission;

    // ------------------------------------------------------------------------
    // 9. TONEMAPPING & GAMMA CORRECTION (Apply in a post-processing step ideally)
    // ------------------------------------------------------------------------
    // Example Reinhard Tonemapping:
    // color = color / (color + vec3(1.0));
    // Example Gamma Correction:
    // color = pow(color, vec3(1.0/2.2));

    // ------------------------------------------------------------------------
    // 10. FINAL OUTPUT
    // ------------------------------------------------------------------------
    FragColor = vec4(color, 1.0f);	
}

