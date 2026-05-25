#version 330 core

struct Material {
    vec3 diffuse_color;
    vec3 specular_color;
    vec3 ambient_color;
    float shininess;
};

uniform Material material;

out vec4 FragColor;

void main()
{
    FragColor = vec4(material.diffuse_color, 1.0);
}