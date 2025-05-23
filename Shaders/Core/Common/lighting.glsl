#ifndef LIGHTING_GLSL
#define LIGHTING_GLSL

// Represents a directional light source (e.g., sun)
struct DirLight {
	vec3 direction;   // Normalized direction vector the light is pointing
	vec3 color;       // Color of the light
	float intensity;  // Brightness of the light
	// std140 layout requires padding here if used in a UBO
};

// Represents a point light source emitting light in all directions
struct PointLight {
	vec3 position;    // World-space position of the light
	vec3 color;       // Color of the light
	float intensity;  // Brightness of the light
	float constant;   // Constant attenuation factor
	float linear;     // Linear attenuation factor
	float quadratic;  // Quadratic attenuation factor
	// std140 layout requires padding here if used in a UBO
};

// Represents a spotlight emitting light in a cone shape
struct SpotLight {
	vec3 position;    // World-space position of the light
	vec3 direction;   // Normalized direction vector the spotlight is pointing
	vec3 color;       // Color of the light
	float intensity;  // Brightness of the light
	float cutOff;     // Cosine of the inner cone angle (smooth edge start)
	float outerCutOff;// Cosine of the outer cone angle (light cutoff)
	float constant;   // Constant attenuation factor
	float linear;     // Linear attenuation factor
	float quadratic;  // Quadratic attenuation factor
	// std140 layout requires padding here if used in a UBO
};

// Maximum number of lights of each type supported in shaders
#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4

#endif // LIGHTING_GLSL