#version 330 core

layout (location = 0) in vec2 position; // Input to raterizer
out vec2 texCoord; // Input for the fragment shader

void main() {
    gl_Position = vec4(position, 1.0, 1.0);
    texCoord = 0.5 * (position + vec2(1,1));
}
