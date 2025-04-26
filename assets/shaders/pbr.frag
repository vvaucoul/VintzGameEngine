#version 450 core

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

// Use layout locations for multiple render targets
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor; // Output for bright pass

uniform vec3 u_ViewPos;

// --- Material ---
uniform sampler2D u_AlbedoMap;    // unit 0
uniform int       u_HasAlbedoMap;
uniform vec3      u_AlbedoColor;

uniform sampler2D u_NormalMap;    // unit 1
uniform int       u_HasNormalMap;

uniform sampler2D u_ORMMap;       // unit 2: R=AO, G=Roughness, B=Metallic
uniform int       u_HasORMMap;

uniform sampler2D u_AOMap;        // unit 3
uniform int       u_HasAOMap;

uniform float     u_Metallic;
uniform float     u_Roughness;
uniform float     u_AO;

// --- Lights ---
struct DirLight {
    vec3 direction;
    vec3 color;
    float intensity;
};
struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float constant, linear, quadratic;
};
#define MAX_POINT_LIGHTS 4
uniform DirLight   dirLight;                       // single directional
uniform int        numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

// === Cook–Torrance ===
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a    = roughness * roughness;
    float a2   = a * a;
    float NdotH= max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = M_PI * denom * denom;
    return a2 / max(denom, 1e-5);
}
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r*r) / 8.0;
    float denom = NdotV * (1.0 - k) + k;
    return NdotV / max(denom, 1e-5);
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float ggx2 = GeometrySchlickGGX(max(dot(N, V), 0.0), roughness);
    float ggx1 = GeometrySchlickGGX(max(dot(N, L), 0.0), roughness);
    return ggx1 * ggx2;
}
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
    // 1) Material fetch
    vec3 albedo = u_HasAlbedoMap == 1
        ? pow(texture(u_AlbedoMap, TexCoords).rgb, vec3(2.2))
        : u_AlbedoColor;

    float metallic, roughness, ao;
    if (u_HasORMMap == 1) {
        vec3 orm = texture(u_ORMMap, TexCoords).rgb;
        ao        = orm.r;
        roughness = orm.g;
        metallic  = orm.b;
    } else {
        metallic  = u_Metallic;
        roughness = u_Roughness;
        ao        = u_HasAOMap == 1
                   ? texture(u_AOMap, TexCoords).r
                   : u_AO;
    }

    // 2) Normal mapping
    vec3 N = normalize(Normal);
    if (u_HasNormalMap == 1) {
        vec3 tn = texture(u_NormalMap, TexCoords).rgb * 2.0 - 1.0;
        N = normalize(TBN * tn);
    }

    vec3 V = normalize(u_ViewPos - FragPos);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    // 3) Direct lighting
    vec3 Lo = vec3(0.0);

    // 3.1 Directional
    {
        vec3 L = normalize(-dirLight.direction);
        vec3 H = normalize(V + L);
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3  F   = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = (1.0 - kS) * (1.0 - metallic);

        float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 1e-5;
        vec3 spec  = (NDF * G * F) / denom;

        float NdotL = max(dot(N, L), 0.0);
        vec3 radiance = dirLight.color * dirLight.intensity;
        Lo += (kD * albedo / M_PI + spec) * radiance * NdotL;
    }

    // 3.2 Point lights
    for (int i = 0; i < numPointLights; ++i) {
        PointLight pl = pointLights[i];
        vec3  L      = normalize(pl.position - FragPos);
        vec3  H      = normalize(V + L);
        float dist   = length(pl.position - FragPos);
        float att    = 1.0 / (pl.constant + pl.linear * dist + pl.quadratic * dist * dist);

        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3  F   = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = (1.0 - kS) * (1.0 - metallic);

        float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 1e-5;
        vec3 spec  = (NDF * G * F) / denom;

        float NdotL = max(dot(N, L), 0.0);
        vec3 radiance = pl.color * pl.intensity * att;
        Lo += (kD * albedo / M_PI + spec) * radiance * NdotL;
    }

    // 4) IBL - Removed calculations
    // vec3 irradiance = texture(u_IrradianceMap, N).rgb;
    // vec3 diffuseIBL = irradiance * albedo;
    //
    // const float MAX_LOD = 4.0;
    // vec3 R            = reflect(-V, N);
    // vec3 prefiltered  = textureLod(u_PrefilterMap, R, roughness * MAX_LOD).rgb;
    // vec2 brdf         = texture(u_BRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    // vec3 specularIBL  = prefiltered * (F0 * brdf.x + brdf.y);
    //
    // vec3 ambient = (diffuseIBL * (1.0 - metallic) + specularIBL) * ao;

    // Simple ambient term (can be adjusted or made configurable)
    vec3 ambient = vec3(0.03) * albedo * ao;

    // 5) Final Color Calculation (before tone mapping/gamma)
    vec3 color = ambient + Lo;

    // --- Output ---
    // Output final HDR color to the first attachment
    FragColor = vec4(color, 1.0);

    // Output bright fragments to the second attachment
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0) // Threshold for bloom
        BrightColor = vec4(color, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

    // Tone mapping and gamma correction are now done in the final post-processing shader
    // // Reinhard tone mapping
    // color = color / (color + vec3(1.0));
    // // Gamma correction
    // color = pow(color, vec3(1.0/2.2));
    // FragColor = vec4(color, 1.0); // Remove this final assignment
}
