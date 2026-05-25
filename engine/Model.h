//
// Created by Volodymyr Avvakumov on 22.05.2026.
//

#ifndef ENGINE_MODEL_H
#define ENGINE_MODEL_H
#include <string>
#include <vector>


#include "defines.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <iostream>
#include <vector>

#include "Mesh.h"

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

  void draw(GLuint shader_program) const {
    for (const Mesh& mesh : meshes_) {
      mesh.draw(shader_program);
    }
  }


  friend std::ostream& operator<<(std::ostream& os, const Model& model) {
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
};

#endif // ENGINE_MODEL_H
