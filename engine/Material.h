//
// Created by Volodymyr Avvakumov on 25.05.2026.
//

#ifndef ENGINE_MATERIAL_H
#define ENGINE_MATERIAL_H
#include "glm/vec3.hpp"

#include <ostream>
#include <string>


struct Material {
    glm::vec3 diffuse_color = glm::vec3(1.0f);
    glm::vec3 specular_color = glm::vec3(1.0f);
    glm::vec3 ambient_color = glm::vec3(0.1f);

    float shininess = 32.0f;

    std::string diffuse_texture_path;
    std::string specular_texture_path;
    std::string normal_texture_path;

    static Material Gold() {
        Material material;
        material.ambient_color = glm::vec3(0.24725f, 0.1995f, 0.0745f);
        material.diffuse_color = glm::vec3(0.75164f, 0.60648f, 0.22648f);
        material.specular_color = glm::vec3(0.628281f, 0.555802f, 0.366065f);
        material.shininess = 51.2f;
        return material;
    }

    static Material Bronze() {
        Material material;
        material.ambient_color = glm::vec3(0.2125f, 0.1275f, 0.054f);
        material.diffuse_color = glm::vec3(0.714f, 0.4284f, 0.18144f);
        material.specular_color = glm::vec3(0.393548f, 0.271906f, 0.166721f);
        material.shininess = 25.6f;
        return material;
    }

    static Material Plastic() {
        Material material;
        material.ambient_color = glm::vec3(0.0f, 0.0f, 0.0f);
        material.diffuse_color = glm::vec3(0.55f, 0.55f, 0.55f);
        material.specular_color = glm::vec3(0.70f, 0.70f, 0.70f);
        material.shininess = 32.0f;
        return material;
    }

    friend std::ostream& operator<<(std::ostream& os, const Material& material);
};


#endif // ENGINE_MATERIAL_H
