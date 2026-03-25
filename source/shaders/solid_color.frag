#version 330 core

uniform vec4 uColor;

in vec3 colorUV;
out vec4 FragColor;

void main()
{
    FragColor = vec4(colorUV.r, colorUV.g, colorUV.b, 1.0);
}