#version 450 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float exposure;

void main() {
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloom    = texture(bloomBlur, TexCoords).rgb;
    hdrColor += bloom; // additive bloom

    // Apply exposure by multiplication
    hdrColor *= exposure;

    // tone mapping (ACES Filmic approximation)
    const float A = 2.51;
    const float B = 0.03;
    const float C = 2.43;
    const float D = 0.59;
    const float E = 0.14;
    // Apply ACES curve to exposed color
    vec3 mapped = clamp((hdrColor*(A*hdrColor+B))/(hdrColor*(C*hdrColor+D)+E), 0.0, 1.0);

    // gamma correct
    mapped = pow(mapped, vec3(1.0/2.2));
    FragColor = vec4(mapped, 1.0);
}
