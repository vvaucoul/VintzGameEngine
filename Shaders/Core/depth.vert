#version 450 core

// Input vertex attribute: Position in object space
layout (location = 0) in vec3 aPos;

// Uniforms for transformations
uniform mat4 model;            // Model matrix: Object space -> World space
uniform mat4 lightSpaceMatrix; // Light's combined view-projection matrix: World space -> Light's clip space

void main() {
	// Calculate the vertex position in the light's clip space.
	// This is used for depth testing from the light's perspective (e.g., shadow mapping).
	// Transformation order: Object -> World -> Light Clip Space
	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
