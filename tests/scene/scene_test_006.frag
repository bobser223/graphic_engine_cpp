#version 330 core

struct Material {
    vec3 diffuse_color;
    vec3 specular_color;
    vec3 ambient_color;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform Material material;
uniform Light light;

uniform sampler2D diffuse_texture;
uniform bool use_texture;

uniform vec3 view_pos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

void main()
{
    vec3 normal = normalize(Normal);

    vec3 base_color = material.diffuse_color;

    if (use_texture) {
        base_color *= texture(diffuse_texture, TexCoord).rgb;
    }

    // ambient
    vec3 ambient = material.ambient_color * base_color;

    // diffuse
    vec3 light_dir = normalize(light.position - FragPos);
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diff * base_color * light.color * light.intensity;

    // Blinn-Phong specular
    vec3 view_dir = normalize(view_pos - FragPos);
    vec3 halfway_dir = normalize(light_dir + view_dir);

    float spec = pow(max(dot(normal, halfway_dir), 0.0), material.shininess);
    vec3 specular = spec * material.specular_color * light.color * light.intensity;

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}