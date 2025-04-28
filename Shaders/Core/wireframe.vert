#version 450 core
layout (location = 0) in vec3 aPos;

// Uniform buffer for matrices (binding = 0)
layout (std140, binding = 0) uniform Matrices {
    mat4 projection;
    mat4 view;
};

// Per-object model matrix
uniform mat4 u_Model;

void main() {
    gl_Position = projection * view * u_Model * vec4(aPos, 1.0);
}
