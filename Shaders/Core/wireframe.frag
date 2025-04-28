#version 450 core
out vec4 FragColor;

uniform vec4 u_WireColor = vec4(1.0, 1.0, 1.0, 1.0); // Default to white

void main() {
    FragColor = u_WireColor;
}
