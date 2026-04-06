#version 330 core

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform vec3 cameraPosition;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
//TODO add texture support

//out vec3 colorUV;
out vec3 ws_normal;

void main()
{

    gl_Position = projectionMatrix * vec4(position, 1.0);
    ws_normal = vec3(modelMatrix * vec4(normal, 0));

}