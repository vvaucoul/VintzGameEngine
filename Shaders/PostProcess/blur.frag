#version 450 core
in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D image;
uniform bool horizontal;
void main() {
    float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
    vec2 tex_offset = 1.0 / textureSize(image, 0); 
    vec3 result = texture(image, TexCoords).rgb * weights[0];
    for(int i = 1; i < 5; ++i) {
        if(horizontal) {
            result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
            result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
        } else {
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weights[i];
            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weights[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
