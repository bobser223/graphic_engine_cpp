//
// Created by Volodymyr Avvakumov on 25.05.2026.
//

#ifndef ENGINE_MATERIAL_H
#define ENGINE_MATERIAL_H
#include <ostream>
#include <string>

#include "glm/vec3.hpp"


struct Material {
    glm::vec3 diffuse_color  = glm::vec3(1.0f);
    glm::vec3 specular_color = glm::vec3(1.0f);
    glm::vec3 ambient_color  = glm::vec3(0.1f);

    float shininess = 32.0f;

    std::string diffuse_texture_path;
    std::string specular_texture_path;
    std::string normal_texture_path;

    friend std::ostream& operator<<(std::ostream& os, const Material& material);
};


#endif //ENGINE_MATERIAL_H
