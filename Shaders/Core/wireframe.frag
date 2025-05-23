#version 450 core

// Output fragment color
out vec4 FragColor;

// Wireframe color (set from application, defaults to white if not set)
uniform vec4 u_WireColor = vec4(1.0);

// Main fragment shader entry point
void main() {
	// Output the wireframe color
	FragColor = u_WireColor;
}
