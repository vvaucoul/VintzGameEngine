#ifndef PBR_LIGHTING_GLSL
#define PBR_LIGHTING_GLSL

// --- PBR Helper Functions ---

const float PI = 3.14159265359;

// Distribution Function (GGX/Trowbridge-Reitz)
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return a2 / max(denom, 1e-5); // Avoid division by zero
}

// Geometry Function (Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0; // For direct lighting
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / max(denom, 1e-5); // Avoid division by zero
}

// Smith's Method (combining Schlick-GGX for view and light directions)
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

// Fresnel Function (Schlick Approximation)
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


// Calculates the PBR contribution from a directional light
// Requires shadowFactor to be passed in
vec3 CalcDirLightPBR(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness, float ao, float shadowFactor) {
    vec3 L = normalize(-light.direction);
    vec3 H = normalize(viewDir + L);
    float NdotL = max(dot(normal, L), 0.0);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, H, roughness);
    float G   = GeometrySmith(normal, viewDir, L, roughness);
    vec3 F0 = mix(vec3(0.04), albedo, metallic); // Base reflectivity
    vec3 F  = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    vec3 kS = F; // Specular reflection coefficient
    vec3 kD = vec3(1.0) - kS; // Diffuse reflection coefficient
    kD *= (1.0 - metallic); // Metallic materials have no diffuse reflection

    // Calculate radiance contribution
    float denom = 4.0 * max(dot(normal, viewDir), 0.0) * NdotL + 1e-5; // Avoid division by zero
    vec3 spec = (NDF * G * F) / denom;

    // Combine diffuse and specular, modulated by light color/intensity and shadow
    vec3 radiance = light.color * light.intensity;
    // Apply AO to diffuse and specular (can be debated, sometimes only applied to ambient)
    return (kD * albedo / PI + spec) * radiance * NdotL * shadowFactor * ao;
}

// Calculates the PBR contribution from a point light
vec3 CalcPointLightPBR(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float metallic, float roughness, float ao) {
    vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(viewDir + L);
    float NdotL = max(dot(normal, L), 0.0);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, H, roughness);
    float G   = GeometrySmith(normal, viewDir, L, roughness);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F  = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= (1.0 - metallic);

    // Attenuation
    float dist = length(light.position - fragPos);
    float att  = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

    // Calculate radiance contribution
    float denom = 4.0 * max(dot(normal, viewDir), 0.0) * NdotL + 1e-5;
    vec3 spec = (NDF * G * F) / denom;

    vec3 radiance = light.color * light.intensity * att;
    return (kD * albedo / PI + spec) * radiance * NdotL * ao;
}

// Calculates the PBR contribution from a spot light
vec3 CalcSpotLightPBR(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float metallic, float roughness, float ao) {
    vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(viewDir + L);
    float NdotL = max(dot(normal, L), 0.0);

    // Spotlight intensity calculation
    float theta = dot(L, normalize(-light.direction)); // Angle between light direction and fragment direction
    float epsilon = light.cutOff - light.outerCutOff;
    float intensityFactor = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, H, roughness);
    float G   = GeometrySmith(normal, viewDir, L, roughness);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F  = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= (1.0 - metallic);

    // Attenuation
    float dist = length(light.position - fragPos);
    float att  = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

    // Calculate radiance contribution
    float denom = 4.0 * max(dot(normal, viewDir), 0.0) * NdotL + 1e-5;
    vec3 spec = (NDF * G * F) / denom;

    vec3 radiance = light.color * light.intensity * att * intensityFactor; // Apply spotlight factor
    return (kD * albedo / PI + spec) * radiance * NdotL * ao;
}


#endif // PBR_LIGHTING_GLSL
