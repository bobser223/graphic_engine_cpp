//
// Created by Volodymyr Avvakumov on 22.05.2026.
//

#ifndef ENGINE_MODEL_H
#define ENGINE_MODEL_H
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
};

#endif // ENGINE_MODEL_H
