#version 450 core

// Output fragment color
out vec4 FragColor;

// Input texture coordinates from vertex shader
in vec2 TexCoords;

// Uniforms for material properties
uniform sampler2D u_AlbedoMap;    // Texture unit 0: Albedo (Base Color) map
uniform int u_HasAlbedoMap;       // Flag: 1 if an albedo map is bound, 0 otherwise
uniform vec3 u_AlbedoColor;       // Fallback color if no albedo map is used

void main() {
	// Determine the base color: use texture if available, otherwise use the uniform color
	vec4 baseColor = vec4(u_AlbedoColor, 1.0); // Default to uniform color with full alpha
	if (u_HasAlbedoMap == 1) {
		baseColor = texture(u_AlbedoMap, TexCoords); // Sample color from the albedo texture
	}

	// Alpha testing: discard fragments that are nearly transparent
	// This prevents rendering pixels with very low alpha values, often used for cutout effects.
	if (baseColor.a < 0.1) {
		discard; // Stop processing this fragment
	}

	// Set the final fragment color
	FragColor = baseColor;
}
