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
uniform float global_ambient_strength;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

void main()
{
    vec3 normal = normalize(Normal);

    if (!gl_FrontFacing) { // для калічної тарілки
        normal = -normal;
    }

    vec3 base_color = material.diffuse_color;

    if (use_texture) {
        base_color *= texture(diffuse_texture, TexCoord).rgb;
    }

    vec3 ambient = global_ambient_strength * base_color;

    vec3 light_dir = normalize(light.position - FragPos);
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diff * base_color * light.color * light.intensity;

    vec3 view_dir = normalize(view_pos - FragPos);
    vec3 halfway_dir = normalize(light_dir + view_dir);

    float shininess = max(material.shininess, 32.0);
    float spec = pow(max(dot(normal, halfway_dir), 0.0), shininess);

    vec3 specular_color = material.specular_color;

    if (length(specular_color) < 0.001) {
        specular_color = vec3(0.3);
    }

    vec3 specular = spec * specular_color * light.color * light.intensity;

    float distance_to_light = length(light.position - FragPos);

    float attenuation = 1.0 / (
    1.0 +
    0.09 * distance_to_light +
    0.032 * distance_to_light * distance_to_light
    );

    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}