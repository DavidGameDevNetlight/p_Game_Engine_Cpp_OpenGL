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
    vec4 colorTexture = textureRect(sceneTexture, gl_FragCoord.xy);
    float averageColor = (colorTexture.r + colorTexture.g + colorTexture.b) / 3.0;
    //FragColor = vec4(vec3(averageColor), 1);
    FragColor = colorTexture;
    //vec4(0.0, 1.0, 0.0, 1.0);
}
