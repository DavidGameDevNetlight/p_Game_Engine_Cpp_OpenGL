#version 330 core

in vec3 colorUV;
in vec3 ws_normal;
out vec4 FragColor;

void main()
{
    //vec3 n = normalize(ws_normal);
    FragColor = vec4(colorUV.r, colorUV.g, colorUV.b, 1.0);
}