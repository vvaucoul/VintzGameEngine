#version 450 core

out vec4 FragColor;

// Input from vertex shader
in vec3 FragPos;   // World space position
in vec3 Normal;    // World space normal
in vec2 TexCoords; // Texture coordinates

// --- Material Uniforms ---
uniform sampler2D u_DiffuseMap;    // Texture unit 0
uniform int       u_HasDiffuseMap; // Boolean flag (0 or 1)
// uniform float     u_Shininess = 32.0f; // Specular shininess (default value, can be set from C++)
const float u_Shininess = 32.0f; // Use const for now if not set per-material

// --- Light Structs ---
struct DirLight {
    vec3 direction;
    vec3 color;
    float intensity;
    // vec3 ambient; // Optional: Ambient contribution
    // vec3 specular; // Optional: Specular color
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
    // vec3 ambient;
    // vec3 specular;
};

// struct SpotLight { ... }; // Add if needed

// --- Light Uniforms ---
uniform DirLight dirLight;

#define MAX_POINT_LIGHTS 4 // Match C++ side if limited
uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

// uniform int numSpotLights;
// uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

// --- Camera Uniform ---
uniform vec3 u_ViewPos; // Camera position in world space

// --- Lighting Calculation Functions ---

// Calculates the color contribution from a directional light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction); // Direction *towards* the light

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * light.intensity;

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), u_Shininess);
    vec3 specular = light.color * spec * light.intensity; // Assuming light color affects specular

    // Ambient (Simple example: use a fraction of light color)
    vec3 ambient = light.color * light.intensity * 0.1; // Basic ambient term

    return (ambient + diffuse + specular);
}

// Calculates the color contribution from a point light
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * light.intensity;

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), u_Shininess);
    vec3 specular = light.color * spec * light.intensity;

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Ambient
    vec3 ambient = light.color * light.intensity * 0.05 * attenuation; // Attenuated ambient

    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

// vec3 CalcSpotLight(...) { ... } // Implement if needed

void main()
{
    vec3 norm = normalize(Normal); // Ensure normal is normalized
    vec3 viewDir = normalize(u_ViewPos - FragPos); // Direction from fragment to view

    // --- Calculate Lighting ---
    // Initialize with a base ambient color (e.g., from scene or global ambient)
    vec3 lightingResult = vec3(0.0); // Start with no light

    // Add directional light contribution
    lightingResult += CalcDirLight(dirLight, norm, viewDir);

    // Add point light contributions
    for(int i = 0; i < numPointLights; ++i) {
        // Check if intensity > 0? Optional optimization
        lightingResult += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    // Add spot light contributions
    // for(int i = 0; i < numSpotLights; ++i) {
    //     lightingResult += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);
    // }

    // --- Get Base Color (Texture or Default) ---
    vec4 baseColor = vec4(1.0); // Default to white if no texture
    if (u_HasDiffuseMap == 1) {
        baseColor = texture(u_DiffuseMap, TexCoords);
    }

    // --- Final Color ---
    // Modulate base color by the calculated lighting
    FragColor = vec4(lightingResult * baseColor.rgb, baseColor.a);

    // Gamma correction (simple example)
    // FragColor.rgb = pow(FragColor.rgb, vec3(1.0/2.2));
}
