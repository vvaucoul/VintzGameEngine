#version 450 core

// ============================================================================
// OUTPUT
// ============================================================================
out vec4 FragColor;

// ============================================================================
// INPUTS FROM VERTEX SHADER
// ============================================================================
in vec3 FragPos;           // World space position
in vec3 Normal;            // World space normal
in vec2 TexCoords;         // Texture coordinates
in mat3 TBN;               // Tangent-Bitangent-Normal matrix
in vec4 FragPosLightSpace; // Fragment position in light space

// ============================================================================
// MATERIAL UNIFORMS
// ============================================================================
uniform sampler2D u_AlbedoMap;    // Texture unit 0: Albedo (Base Color)
uniform sampler2D u_NormalMap;    // Texture unit 1: Normal Map
uniform sampler2D u_AOMap;        // Texture unit 3: Ambient Occlusion (optional)
uniform sampler2D shadowMap;      // Texture unit 4: Shadow Map

uniform int u_HasAlbedoMap;       // 1 if albedo map is used
uniform int u_HasNormalMap;       // 1 if normal map is used
uniform int u_HasAOMap;           // 1 if AO map is used

uniform vec3 u_AlbedoColor;       // Fallback albedo color
uniform float u_Metallic;         // Fallback metallic value
uniform float u_Roughness;        // Fallback roughness value
uniform float u_AO;               // Fallback AO value

// ============================================================================
// LIGHT STRUCTS & UNIFORMS
// ============================================================================
struct DirLight {
	vec3 direction;
	vec3 color;
	float intensity;
};

struct PointLight {
	vec3 position;
	vec3 color;
	float intensity;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	vec3 color;
	float intensity;
	float cutOff;       // Cosine of inner angle
	float outerCutOff;  // Cosine of outer angle
	float constant;
	float linear;
	float quadratic;
};

uniform DirLight dirLight;

#define MAX_POINT_LIGHTS 4
uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

#define MAX_SPOT_LIGHTS 4
uniform int numSpotLights;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

// ============================================================================
// CAMERA & SHADOW UNIFORMS
// ============================================================================
uniform vec3 u_ViewPos;        // Camera position in world space
uniform mat4 lightSpaceMatrix; // Light's view-projection matrix

// ============================================================================
// CONSTANTS
// ============================================================================
const float PI = 3.14159265359;

// ============================================================================
// PBR HELPER FUNCTIONS
// ============================================================================

// GGX/Trowbridge-Reitz Normal Distribution Function (NDF)
float DistributionGGX(vec3 N, vec3 H, float roughness) {
	float a      = roughness * roughness;
	float a2     = a * a;
	float NdotH  = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;
	float nom    = a2;
	float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
	denom        = PI * denom * denom;
	return nom / max(denom, 1e-7); // Prevent divide by zero
}

// Schlick-GGX Geometry Function (for direct lighting)
float GeometrySchlickGGX(float NdotV, float roughness) {
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;
	float nom   = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	return nom / max(denom, 1e-7);
}

// Smith's Geometry Function (combines view and light)
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggxV = GeometrySchlickGGX(NdotV, roughness);
	float ggxL = GeometrySchlickGGX(NdotL, roughness);
	return ggxV * ggxL;
}

// Fresnel Schlick approximation
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// ============================================================================
// SHADOW CALCULATION (Directional Light, PCF)
// ============================================================================
float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
	// Transform fragment position to [0,1] light space
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	// If outside shadow map, no shadow
	if (projCoords.z > 1.0) return 0.0;

	// Shadow bias to reduce shadow acne
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

	// Percentage Closer Filtering (3x3 kernel)
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; ++x) {
		for (int y = -1; y <= 1; ++y) {
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += (projCoords.z - bias) > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;
	return 1.0 - shadow;
}

// ============================================================================
// LIGHTING CALCULATION FUNCTIONS
// ============================================================================

// Directional Light PBR
vec3 CalcDirLightPBR(
	DirLight light, vec3 normal, vec3 viewDir,
	vec3 albedo, float metallic, float roughness, float ao
) {
	vec3 L = normalize(-light.direction);
	vec3 H = normalize(viewDir + L);
	float NdotL = max(dot(normal, L), 0.0);

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(normal, H, roughness);
	float G   = GeometrySmith(normal, viewDir, L, roughness);
	vec3 F0   = mix(vec3(0.04), albedo, metallic);
	vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);

	vec3 kS = F;
	vec3 kD = (1.0 - kS) * (1.0 - metallic);

	vec3 numerator   = NDF * G * F;
	float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * NdotL + 1e-4;
	vec3 specular    = numerator / denominator;

	// Shadow calculation
	float shadow = CalculateShadow(FragPosLightSpace, normal, L);

	// Final radiance
	vec3 Lo = light.color * light.intensity * NdotL;
	return (kD * albedo / PI + specular) * Lo * ao * shadow;
}

// Point Light PBR
vec3 CalcPointLightPBR(
	PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,
	vec3 albedo, float metallic, float roughness, float ao
) {
	vec3 L = normalize(light.position - fragPos);
	vec3 H = normalize(viewDir + L);
	float NdotL = max(dot(normal, L), 0.0);

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(normal, H, roughness);
	float G   = GeometrySmith(normal, viewDir, L, roughness);
	vec3 F0   = mix(vec3(0.04), albedo, metallic);
	vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);

	vec3 kS = F;
	vec3 kD = (1.0 - kS) * (1.0 - metallic);

	vec3 numerator   = NDF * G * F;
	float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * NdotL + 1e-4;
	vec3 specular    = numerator / denominator;

	// Attenuation
	float distance    = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// Final radiance
	vec3 Lo = light.color * light.intensity * NdotL * attenuation;
	return (kD * albedo / PI + specular) * Lo * ao;
	// Note: Point light shadows not implemented
}

// Spot Light PBR
vec3 CalcSpotLightPBR(
	SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir,
	vec3 albedo, float metallic, float roughness, float ao
) {
	vec3 L = normalize(light.position - fragPos);
	float theta = dot(L, normalize(-light.direction));

	// Check if inside spotlight cone
	if (theta > light.outerCutOff) {
		vec3 H = normalize(viewDir + L);
		float NdotL = max(dot(normal, L), 0.0);

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(normal, H, roughness);
		float G   = GeometrySmith(normal, viewDir, L, roughness);
		vec3 F0   = mix(vec3(0.04), albedo, metallic);
		vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);

		vec3 kS = F;
		vec3 kD = (1.0 - kS) * (1.0 - metallic);

		vec3 numerator   = NDF * G * F;
		float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * NdotL + 1e-4;
		vec3 specular    = numerator / denominator;

		// Attenuation
		float distance    = length(light.position - fragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

		// Smooth edge (spotlight falloff)
		float epsilon        = light.cutOff - light.outerCutOff;
		float intensityFactor = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

		// Final radiance
		vec3 Lo = light.color * light.intensity * NdotL * attenuation * intensityFactor;
		return (kD * albedo / PI + specular) * Lo * ao;
		// Note: Spot light shadows not implemented
	}
	return vec3(0.0);
}

// ============================================================================
// MAIN FRAGMENT SHADER ENTRY POINT
// ============================================================================
void main() {
	// ------------------------------------------------------------------------
	// MATERIAL PROPERTY FETCH
	// ------------------------------------------------------------------------
	vec3 albedo = u_AlbedoColor;
	float alpha = 1.0; // Default alpha

	if (u_HasAlbedoMap == 1) {
		vec4 albedoSample = texture(u_AlbedoMap, TexCoords);
		albedo = albedoSample.rgb;
		alpha  = albedoSample.a; // Get alpha from texture

		// Gamma correct if texture is sRGB
		albedo = pow(albedo, vec3(2.2));
	}

	float metallic  = u_Metallic;
	float roughness = u_Roughness;
	float ao        = u_AO;

	// If ORM map was used, sample here (currently not used)
	// if (u_HasORMMap == 1) {
	//     vec3 orm = texture(u_ORMMap, TexCoords).rgb;
	//     ao        = orm.r;
	//     roughness = orm.g;
	//     metallic  = orm.b;
	// }

	// Separate AO map
	if (u_HasAOMap == 1) {
		ao = texture(u_AOMap, TexCoords).r;
	}

	// ------------------------------------------------------------------------
	// NORMAL FETCH (TANGENT SPACE NORMAL MAPPING)
	// ------------------------------------------------------------------------
	vec3 N = normalize(Normal);
	if (u_HasNormalMap == 1) {
		vec3 tangentNormal = texture(u_NormalMap, TexCoords).rgb * 2.0 - 1.0;
		N = normalize(TBN * tangentNormal);
	}

	// ------------------------------------------------------------------------
	// VIEW DIRECTION
	// ------------------------------------------------------------------------
	vec3 V = normalize(u_ViewPos - FragPos);

	// ------------------------------------------------------------------------
	// LIGHTING ACCUMULATION
	// ------------------------------------------------------------------------
	vec3 Lo = vec3(0.0);

	// Directional light
	Lo += CalcDirLightPBR(dirLight, N, V, albedo, metallic, roughness, ao);

	// Point lights
	for (int i = 0; i < numPointLights; ++i) {
		Lo += CalcPointLightPBR(pointLights[i], N, FragPos, V, albedo, metallic, roughness, ao);
	}

	// Spot lights
	for (int i = 0; i < numSpotLights; ++i) {
		Lo += CalcSpotLightPBR(spotLights[i], N, FragPos, V, albedo, metallic, roughness, ao);
	}

	// ------------------------------------------------------------------------
	// AMBIENT TERM (AO ONLY, NO IBL)
	// ------------------------------------------------------------------------
	vec3 ambient = vec3(0.03 * ao);
	vec3 color = ambient * albedo + Lo;

	// ------------------------------------------------------------------------
	// OUTPUT (HDR, NO GAMMA CORRECTION HERE)
	// ------------------------------------------------------------------------

	// Discard fragment if alpha is very low (adjust threshold as needed)
	// This is crucial for sharp transparency cutouts like billboards
	if (alpha < 0.1) {
		discard;
	}

	FragColor = vec4(color, alpha);

	// ------------------------------------------------------------------------
	// DEBUG OUTPUTS (UNCOMMENT TO VISUALIZE)
	// ------------------------------------------------------------------------
	// FragColor = vec4(albedo, 1.0);
	// FragColor = vec4(N * 0.5 + 0.5, 1.0); // Visualize normals
	// FragColor = vec4(metallic, metallic, metallic, 1.0);
	// FragColor = vec4(roughness, roughness, roughness, 1.0);
	// FragColor = vec4(ao, ao, ao, 1.0);
	// float shadow = CalculateShadow(FragPosLightSpace, N, normalize(-dirLight.direction));
	// FragColor = vec4(vec3(shadow), 1.0); // Visualize shadow
}