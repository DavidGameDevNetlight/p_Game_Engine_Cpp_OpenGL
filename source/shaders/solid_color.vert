#version 330 core

uniform vec3 uOffSet;

uniform mat4 projectionMatrix;
uniform vec3 cameraPosition;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 colorUV;

void main()
{
    // This is a hardcoded rotation matrix we use to have the camera look slightly downward
    mat3 V = mat3(
    1.0, 0.0, 0.0,
    0.0, 0.97, 0.26,
    0.0, -0.26, 0.97);

    vec4 pos = vec4((V * position.xyz) - cameraPosition.xyz, 1);
    gl_Position = projectionMatrix * pos;

    colorUV = color;
}