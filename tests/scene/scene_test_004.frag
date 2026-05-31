#version 330 core

struct Material {
    vec3 diffuse_color;
    vec3 specular_color;
    vec3 ambient_color;
    float shininess;
};

uniform Material material;
uniform sampler2D diffuse_texture;
uniform bool use_texture;

in vec2 TexCoord;

out vec4 FragColor;

void main()
{
    vec4 base_color = vec4(material.diffuse_color, 1.0);

    if (use_texture) {
        base_color *= texture(diffuse_texture, TexCoord);
    }

    FragColor = base_color;
}