#version 330 core

layout (location = 0) in vec2 position;

out vec2 texCoords;

void main() {
    vec4 normalized_device_coordinates = vec4(position.x, position.y, 1.0, 1.0);
    texCoords   = 0.5 * ( position.xy + vec2(1));
    gl_Position = normalized_device_coordinates;
}
