//
// Created by Volodymyr Avvakumov on 22.05.2026.
//

#include "Model.h"

#include "Logger.h"

void Model::draw(GLuint shader_program) const {
    LOG_TRACE("Drawing model from directory=", directory_, ", meshes=", meshes_.size());
    for (const Mesh& mesh : meshes_) {
        mesh.draw(shader_program);
    }
}

std::ostream& operator<<(std::ostream& os, const Model& model) {
    os << "Model{directory=\"" << model.directory_ << "\", meshes=[";
    for (std::size_t i = 0; i < model.meshes_.size(); ++i) {
        if (i != 0) {
            os << ", ";
        }
        os << model.meshes_[i];
    }
    os << "]}";
    return os;
}
