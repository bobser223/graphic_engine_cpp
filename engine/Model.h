//
// Created by Volodymyr Avvakumov on 22.05.2026.
//

#ifndef ENGINE_MODEL_H
#define ENGINE_MODEL_H
#include "Mesh.h"
#include "defines.h"

#include <iostream>
#include <string>
#include <vector>

class Model {

public:
    Model() = default;
    ~Model() = default;

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    Model(Model&&) noexcept = default;
    Model& operator=(Model&&) noexcept = default;


    std::vector<Mesh> meshes_;
    std::string directory_;

    void draw(GLuint shader_program) const;

    friend std::ostream& operator<<(std::ostream& os, const Model& model);
};

#endif // ENGINE_MODEL_H
