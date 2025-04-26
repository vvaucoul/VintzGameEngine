#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;

layout(std140, binding = 0) uniform Matrices {
    mat4 u_Projection;
    mat4 u_View;
};

uniform mat4 u_Model;

void main() {
    // Calculate world position
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    FragPos = worldPos.xyz;

    // Calculate world space normal and TBN matrix
    // Normal matrix (inverse transpose of the upper-left 3x3 model matrix)
    // Simplification: Assume uniform scaling or no scaling, use mat3(u_Model)
    mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
    Normal = normalize(normalMatrix * a_Normal);

    // Calculate TBN matrix (Requires Tangent attribute)
    // If tangents are available (location 3):
    // vec3 T = normalize(normalMatrix * a_Tangent);
    // vec3 N = Normal; // Already calculated
    // vec3 B = normalize(cross(N, T)); // Calculate Bitangent
    // TBN = mat3(T, B, N);
    // If tangents are NOT available, create a placeholder TBN.
    // This won't work correctly for normal mapping without tangents.
    vec3 tangent = vec3(1.0, 0.0, 0.0); // Placeholder
    vec3 bitangent = normalize(cross(Normal, tangent)); // Placeholder
    tangent = normalize(cross(bitangent, Normal)); // Re-orthogonalize tangent
    TBN = mat3(tangent, bitangent, Normal);


    // Pass texture coordinates
    TexCoords = a_TexCoords;

    // Calculate final clip space position
    gl_Position = u_Projection * u_View * worldPos;
}
