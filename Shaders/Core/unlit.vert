#version 450 core

// Vertex attributes
layout (location = 0) in vec3 aPos;        // Vertex position (object space)
layout (location = 2) in vec2 aTexCoords;  // Texture coordinates

// Camera matrices (shared via UBO, binding = 0)
layout (std140, binding = 0) uniform Matrices {
	mat4 projection; // Projection matrix
	mat4 view;       // View matrix
};

// Per-object transform
uniform mat4 u_Model; // Model matrix

// Output to fragment shader
out vec2 TexCoords;

void main() {
	// Transform vertex to clip space
	gl_Position = projection * view * u_Model * vec4(aPos, 1.0);
	// Pass texture coordinates through
	TexCoords = aTexCoords;
}
