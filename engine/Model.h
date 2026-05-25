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

  std::vector<Mesh> meshes_;
  std::string directory_;

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
