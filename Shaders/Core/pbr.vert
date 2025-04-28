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
out vec3 FragPos;           // World space position
out vec3 Normal;            // World space normal (geometric, interpolated)
out vec2 TexCoords;         // Texture coordinates (interpolated)
out mat3 TBN;               // World -> Tangent space matrix (interpolated)
out vec4 FragPosLightSpace; // Position in light's clip space (interpolated)
out vec3 WorldTangent;      // World space tangent vector (interpolated)
out vec3 WorldBitangent;    // World space bitangent vector (interpolated)

// ============================================================================
// UNIFORMS
// ============================================================================
// Camera matrices (usually bound via UBO)
layout(std140, binding = 0) uniform Matrices {
    mat4 u_Projection;
    mat4 u_View;
};

// Model and light matrices
uniform mat4 u_Model;          // Model transformation matrix
uniform mat4 lightSpaceMatrix; // Light's view-projection matrix (for shadows)

// ============================================================================
// MAIN VERTEX SHADER FUNCTION
// ============================================================================
void main() {
    // --- Calculate World Position ---
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    FragPos = worldPos.xyz;

    // --- Calculate Normal Matrix ---
    // Used to transform normals, tangents, bitangents correctly,
    // especially with non-uniform scaling.
    mat3 normalMatrix = transpose(inverse(mat3(u_Model)));

    // --- Calculate World Space Normal, Tangent, Bitangent ---
    // Transform direction vectors from model space to world space
    vec3 N = normalize(normalMatrix * a_Normal);
    vec3 T = normalize(normalMatrix * a_Tangent);
    vec3 B = normalize(normalMatrix * a_Bitangent); // Transform original bitangent

    // --- Orthogonalize TBN Basis (Gram-Schmidt) ---
    // Ensure the basis vectors passed to the fragment shader are orthogonal,
    // especially important after interpolation and potential normal mapping.
    // 1. Keep the Normal (N) as calculated.
    // 2. Make Tangent (T) orthogonal to Normal.
    T = normalize(T - dot(T, N) * N);
    // 3. Recalculate Bitangent (B) to be orthogonal to both N and T.
    //    This also ensures the basis remains right-handed (or left-handed, consistently).
    B = cross(N, T);
    // Note: Depending on the original bitangent calculation (tool/importer),
    // you might need B = cross(T, N) if the original winding order was different.
    // Check visual results, especially with normal maps.

    // --- Pass World Space Vectors ---
    // Pass the final, orthogonalized world-space vectors.
    Normal = N;
    WorldTangent = T;
    WorldBitangent = B;

    // --- Calculate TBN Matrix (World -> Tangent Space) ---
    // This matrix is needed by the fragment shader to transform vectors
    // (like the view direction or light direction) from world space into tangent space,
    // primarily for parallax mapping and potentially other tangent-space effects.
    // It's the transpose of the matrix formed by T, B, N as columns.
    TBN = transpose(mat3(T, B, N));

    // --- Pass Texture Coordinates ---
    TexCoords = a_TexCoords;

    // --- Calculate Fragment Position in Light Space ---
    // Used for shadow mapping calculations in the fragment shader.
    FragPosLightSpace = lightSpaceMatrix * worldPos;

    // --- Calculate Final Clip Space Position ---
    // The final output position required by OpenGL.
    gl_Position = u_Projection * u_View * worldPos;
}
