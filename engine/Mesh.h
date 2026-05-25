
#pragma once
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"


#include "defines.h"
#include "Material.h"


struct Vertex {
  glm::vec3 position_;
  glm::vec3 normal_;
  glm::vec2 tex_coord_;
};


class Mesh {
public:


  Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<idx>& indices,
    const Material& material): vertices_(vertices), indices_(indices), material_(material) {}

  ~Mesh() = default;







  std::vector<Vertex> vertices_;
  std::vector<idx> indices_;
  Material material_;


private:
  uint VAO_;
  uint VBO_;
  uint EBO_;
};
