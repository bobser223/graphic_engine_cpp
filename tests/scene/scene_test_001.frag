#version 330 core

struct Material {
    vec3 diffuse_color;
    vec3 specular_color;
    vec3 ambient_color;
    float shininess;
};

uniform Material material;
uniform sampler2D diffuse_texture;

in vec2 TexCoord;

out vec4 FragColor;

void main()
{
    vec4 texel = texture(diffuse_texture, TexCoord);
    FragColor = vec4(material.diffuse_color, 1.0) * texel;
}
