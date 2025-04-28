#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_AlbedoMap;    // Texture unit 0: Albedo (Base Color)
uniform int u_HasAlbedoMap;       // 1 if albedo map is used
uniform vec3 u_AlbedoColor;       // Fallback albedo color

void main() {
    vec4 baseColor = vec4(u_AlbedoColor, 1.0);
    if (u_HasAlbedoMap == 1) {
        baseColor = texture(u_AlbedoMap, TexCoords);
    }

    // Discard fragment if alpha is very low
    if (baseColor.a < 0.1) {
        discard;
    }

    FragColor = baseColor;
}
