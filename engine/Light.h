//
// Created by Volodymyr Avvakumov on 06.06.2026.
//

#ifndef ENGINE_LIGHT_H
#define ENGINE_LIGHT_H

#define ENGINE_LIGHT_H

#include <glm/vec3.hpp>

struct Light {
    glm::vec3 position = glm::vec3(2.0f, 4.0f, 2.0f);
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;

    Light(const glm::vec3& position, const glm::vec3& color, const float intensity)
        : position(position)
        , color(color)
        , intensity(intensity) {}
    Light() = default;
    ~Light() = default;
};


#endif // ENGINE_LIGHT_H
