#version 450 core
in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D sceneBright;
void main() {
    vec3 color = texture(sceneBright, TexCoords).rgb;
    float brightness = max(max(color.r,color.g),color.b);
    if(brightness > 1.0)
        FragColor = vec4(color,1.0);
    else
        FragColor = vec4(0.0);
}
