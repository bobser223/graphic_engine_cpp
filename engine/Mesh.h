
#pragma once
#include <ostream>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"


#include "defines.h"
#include "Material.h"


struct Vertex {
  glm::vec3 position_;
  glm::vec3 normal_;
  glm::vec2 tex_coord_;

  friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex) {
    os << "Vertex{position=("
       << vertex.position_.x << ", " << vertex.position_.y << ", " << vertex.position_.z
       << "), normal=("
       << vertex.normal_.x << ", " << vertex.normal_.y << ", " << vertex.normal_.z
       << "), tex_coord=("
       << vertex.tex_coord_.x << ", " << vertex.tex_coord_.y
       << ")}";
    return os;
  }
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
  Material material_; //TODO: make this through an shered pointer

  friend std::ostream& operator<<(std::ostream& os, const Mesh& mesh) {
    os << "Mesh{vertices=[";
    for (std::size_t i = 0; i < mesh.vertices_.size(); ++i) {
      if (i != 0) {
        os << ", ";
      }
      os << mesh.vertices_[i];
    }

    os << "], indices=[";
    for (std::size_t i = 0; i < mesh.indices_.size(); ++i) {
      if (i != 0) {
        os << ", ";
      }
      os << mesh.indices_[i];
    }

    os << "], material=" << mesh.material_ << "}";
    return os;
  }

private:
  uint VAO_;
  uint VBO_;
  uint EBO_;
};
