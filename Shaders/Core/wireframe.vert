#version 450 core

// Input vertex attribute: Position (location = 0)
layout (location = 0) in vec3 aPos;

// Uniform Buffer Object (UBO) for common matrices
// Bound to binding point 0, using std140 layout for memory alignment
layout (std140, binding = 0) uniform Matrices {
	mat4 projection; // Projection matrix (Camera space to Clip space)
	mat4 view;       // View matrix (World space to Camera space)
};

// Per-object uniform: Model matrix (Object space to World space)
uniform mat4 u_Model;

// Main entry point for the vertex shader
void main() {
	// Calculate the final vertex position in clip space
	// Transformation order: Object -> World -> Camera -> Clip
	// Note: GLM uses column-major matrices, so multiplication order is P * V * M * v
	gl_Position = projection * view * u_Model * vec4(aPos, 1.0);
}
