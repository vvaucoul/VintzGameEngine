#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
// layout (location = 3) in vec3 aTangent; // Add if tangents are available and needed

// Uniform Buffer Object for matrices
layout (std140, binding = 0) uniform Matrices {
    mat4 projection;
    mat4 view;
};

uniform mat4 u_Model;

out VS_OUT {
    vec3 FragPos;   // World space position
    vec3 Normal;    // World space normal
    vec2 TexCoords;
    // mat3 TBN;    // Optional: Tangent-to-World matrix if doing normal mapping here
} vs_out;

void main() {
    mat4 viewModel = view * u_Model;
    vec4 worldPos = u_Model * vec4(aPos, 1.0);

    vs_out.FragPos = vec3(worldPos);
    // Calculate world normal (inverse transpose for non-uniform scaling)
    vs_out.Normal = normalize(mat3(transpose(inverse(u_Model))) * aNormal);
    vs_out.TexCoords = aTexCoords;

    // Optional: Calculate TBN matrix if needed for normal mapping in GBuffer pass
    // vec3 T = normalize(mat3(u_Model) * aTangent);
    // vec3 N = vs_out.Normal;
    // T = normalize(T - dot(T, N) * N); // Gram-Schmidt orthogonalize
    // vec3 B = cross(N, T);
    // vs_out.TBN = mat3(T, B, N);

    gl_Position = projection * viewModel * vec4(aPos, 1.0);
}