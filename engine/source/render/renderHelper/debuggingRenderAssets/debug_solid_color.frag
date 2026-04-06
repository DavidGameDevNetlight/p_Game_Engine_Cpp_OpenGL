#version 330 core

uniform vec3 debugSolidColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(debugSolidColor, 1.0);
}
