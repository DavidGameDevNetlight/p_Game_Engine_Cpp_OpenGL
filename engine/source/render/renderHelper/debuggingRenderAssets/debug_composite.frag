#version 330 core

uniform sampler2D sceneTexture;
uniform sampler2D depthTexture;

in  vec2 texCoords;
out vec4 FragColor;

/**
* Helper function to sample with pixel coordinates, e.g., (511.5, 12.75)
* This functionality is similar to using sampler2DRect.
* TexelFetch only work with integer coordinates and do not perform bilinerar filtering.
*/
vec4 textureRect(in sampler2D tex, vec2 rectangleCoord)
{
    return texture(tex, rectangleCoord / textureSize(tex, 0));
}

void main() {
    FragColor = textureRect(sceneTexture, gl_FragCoord.xy);
    //FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
