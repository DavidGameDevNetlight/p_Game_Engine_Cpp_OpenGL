#version 330 core

uniform vec4 uColor;
//precision highp float;

uniform vec3 material_color     = vec3(0.0);
uniform vec3 material_emission  = vec3(0.0);
uniform vec3 light_direction    = vec3(0.0);
uniform vec3 solid_color        = vec3(0.0);
uniform bool is_solid_color     = false;
//uniform int has_color_texture = 0;
//layout(binding = 0) uniform sampler2D color_texture;

//uniform int has_emission_texture = 0;
//layout(binding = 5) uniform sampler2D emission_texture;

//in vec3 colorUV;
in vec3 ws_normal;
out vec4 FragColor;

void main()
{

    vec3 n = normalize(ws_normal);
    vec4 color = vec4(material_color, 1.0);

    /*if(has_color_texture == 1)
    {
        color = texture(color_texture, texCoord.xy);
    }*/

    vec4 emission = vec4(material_emission, 0);
    /*if(has_emission_texture == 1)
    {
        emission = texture(emission_texture, texCoord.xy);
    }*/

    //const vec3 lightDir = normalize(vec3(-0.74, -1, 0.68));
    //color = vec4(0.0, 0.0, 1.0, 1.0);
    FragColor = color * max(dot(n, -light_direction), 0.3) + emission;
    //FragColor = vec4(1.0,0.0,0.0,1.0); // debug

}