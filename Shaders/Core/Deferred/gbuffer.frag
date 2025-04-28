#version 450 core
layout (location = 0) out vec4 gPositionMetallic; // World Pos (xyz), Metallic (w)
layout (location = 1) out vec4 gNormalRoughness;  // World Normal (xyz), Roughness (w)
layout (location = 2) out vec4 gAlbedoAO;         // Albedo Color (rgb), AO (a)
// layout (location = 3) out vec4 gEmissiveSpecular; // Optional: Emissive (rgb), Specular (a)

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    // mat3 TBN; // Optional
} fs_in;

// Material properties (passed via uniforms or sampled from textures)
// Assuming MaterialPBR structure is reflected here
uniform vec3  u_Material_AlbedoColor;
uniform float u_Material_Metallic;
uniform float u_Material_Roughness;
uniform float u_Material_AO;
// uniform float u_Material_Specular; // If needed
// uniform vec3  u_Material_EmissiveColor; // If needed

uniform sampler2D u_Material_AlbedoMap;
uniform sampler2D u_Material_NormalMap;
uniform sampler2D u_Material_MetallicMap;
uniform sampler2D u_Material_RoughnessMap;
uniform sampler2D u_Material_AOMap;
// uniform sampler2D u_Material_SpecularMap; // If needed
// uniform sampler2D u_Material_EmissiveMap; // If needed

uniform bool u_Material_HasAlbedoMap;
uniform bool u_Material_HasNormalMap;
uniform bool u_Material_HasMetallicMap;
uniform bool u_Material_HasRoughnessMap;
uniform bool u_Material_HasAOMap;
// uniform bool u_Material_HasSpecularMap; // If needed
// uniform bool u_Material_HasEmissiveMap; // If needed


// Function to get normal from normal map (tangent space -> world space)
// Note: This requires tangents to be passed from vertex shader and TBN matrix calculated
// For simplicity now, we'll just use the vertex normal. Add this back if needed.
/*
vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(u_Material_NormalMap, fs_in.TexCoords).xyz * 2.0 - 1.0;
    // Transform tangent normal to world space using TBN matrix
    return normalize(fs_in.TBN * tangentNormal);
}
*/

void main() {
    // --- Position ---
    // Store world position in RGB. We might store depth or another value in W if needed.
    gPositionMetallic.xyz = fs_in.FragPos;

    // --- Normal ---
    vec3 normal = normalize(fs_in.Normal);
    // if (u_Material_HasNormalMap) {
    //     normal = getNormalFromMap(); // Requires TBN matrix and tangents
    // }
    gNormalRoughness.xyz = normal; // Store world normal

    // --- Albedo ---
    vec3 albedo = u_Material_AlbedoColor;
    if (u_Material_HasAlbedoMap) {
        albedo = texture(u_Material_AlbedoMap, fs_in.TexCoords).rgb;
    }
    gAlbedoAO.rgb = albedo;

    // --- Metallic ---
    float metallic = u_Material_Metallic;
    if (u_Material_HasMetallicMap) {
        metallic = texture(u_Material_MetallicMap, fs_in.TexCoords).r; // Assuming metallic is in R channel
    }
    gPositionMetallic.w = metallic; // Store metallic in Position's W component

    // --- Roughness ---
    float roughness = u_Material_Roughness;
    if (u_Material_HasRoughnessMap) {
        roughness = texture(u_Material_RoughnessMap, fs_in.TexCoords).r; // Assuming roughness is in R channel
    }
    gNormalRoughness.w = roughness; // Store roughness in Normal's W component

    // --- Ambient Occlusion ---
    float ao = u_Material_AO;
    if (u_Material_HasAOMap) {
        ao = texture(u_Material_AOMap, fs_in.TexCoords).r; // Assuming AO is in R channel
    }
    gAlbedoAO.a = ao; // Store AO in Albedo's Alpha component

    // --- Optional: Emissive / Specular ---
    // float specular = u_Material_Specular;
    // if (u_Material_HasSpecularMap) { ... }
    // vec3 emissive = u_Material_EmissiveColor;
    // if (u_Material_HasEmissiveMap) { ... }
    // gEmissiveSpecular.rgb = emissive;
    // gEmissiveSpecular.a = specular;
}