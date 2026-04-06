#version 330 core

uniform mat4 modelViewProjectionMatrix;
layout (location = 0) in vec3 position;

void main() {
    vec4 normalized_device_coordinates = modelViewProjectionMatrix * vec4(position.xyz, 1.0);
    gl_Position = normalized_device_coordinates;
}
