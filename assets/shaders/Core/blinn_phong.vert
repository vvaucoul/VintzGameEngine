#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;

// Output to fragment shader
out vec3 FragPos;   // World space position
out vec3 Normal;    // World space normal
out vec2 TexCoords; // Texture coordinates

// Bloc UBO pour les matrices View et Projection (binding = 0)
layout(std140, binding = 0) uniform Matrices
{
    mat4 u_Projection; // Added Projection
    mat4 u_View;       // Added View
};

uniform mat4 u_Model; // Model matrix uniform

void main()
{
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    FragPos = vec3(worldPos); // Pass world position

    // Calculate world normal (correct transformation for non-uniform scaling)
    Normal = normalize(mat3(transpose(inverse(u_Model))) * a_Normal);

    TexCoords = a_TexCoords; // Pass texture coordinates

    gl_Position = u_Projection * u_View * worldPos; // Use separate View and Projection from UBO
}
