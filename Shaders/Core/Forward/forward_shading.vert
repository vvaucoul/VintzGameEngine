#version 450 core

// ============================================================================
// INPUT VERTEX ATTRIBUTES
// ============================================================================
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;
layout(location = 3) in vec3 a_Tangent;   // Input tangent (model space)
layout(location = 4) in vec3 a_Bitangent; // Input bitangent (model space)

// ============================================================================
// OUTPUTS TO FRAGMENT SHADER
// ============================================================================
out VS_OUT {
    vec3 FragPos;           // World space position
    vec3 Normal;            // World space normal (geometric, interpolated)
    vec2 TexCoords;         // Texture coordinates (interpolated)
    mat3 TBN;               // World -> Tangent space matrix (interpolated)
    vec4 FragPosLightSpace; // Position in light's clip space (interpolated, for directional shadow)
    vec3 WorldTangent;      // World space tangent vector (interpolated)
    vec3 WorldBitangent;    // World space bitangent vector (interpolated)
} vs_out;

// ============================================================================
// UNIFORMS
// ============================================================================
// Camera matrices (bound via UBO)
layout(std140, binding = 0) uniform Matrices {
    mat4 u_Projection;
    mat4 u_View;
};

// Model and light matrices
uniform mat4 u_Model;          // Model transformation matrix
uniform mat4 lightSpaceMatrix; // Directional Light's view-projection matrix (for shadows)

// ============================================================================
// MAIN VERTEX SHADER FUNCTION
// ============================================================================
void main() {
    // --- Calculate World Position ---
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    vs_out.FragPos = worldPos.xyz;

    // --- Calculate Normal Matrix ---
    mat3 normalMatrix = transpose(inverse(mat3(u_Model)));

    // --- Calculate World Space Normal, Tangent, Bitangent ---
    vec3 N = normalize(normalMatrix * a_Normal);
    vec3 T = normalize(normalMatrix * a_Tangent);
    vec3 B = normalize(normalMatrix * a_Bitangent);

    // --- Orthogonalize TBN Basis (Gram-Schmidt) ---
    T = normalize(T - dot(T, N) * N);
    B = cross(N, T); // Recalculate B for orthogonality

    // --- Pass World Space Vectors ---
    vs_out.Normal = N;
    vs_out.WorldTangent = T;
    vs_out.WorldBitangent = B;

    // --- Calculate TBN Matrix (World -> Tangent Space) ---
    vs_out.TBN = transpose(mat3(T, B, N));

    // --- Pass Texture Coordinates ---
    vs_out.TexCoords = a_TexCoords;

    // --- Calculate Fragment Position in Light Space (for directional shadow) ---
    vs_out.FragPosLightSpace = lightSpaceMatrix * worldPos;

    // --- Calculate Final Clip Space Position ---
    gl_Position = u_Projection * u_View * worldPos;
}