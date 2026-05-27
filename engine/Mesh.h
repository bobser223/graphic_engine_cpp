
#pragma once
#include <ostream>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"


#include "defines.h"
#include "Material.h"
#include "glad/glad.h"


struct Vertex {
  glm::vec3 position_;
  glm::vec3 normal_;
  glm::vec2 tex_coord_;

  friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex);
};


class Mesh {
public:


  Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<idx>& indices,
    Material material);

  ~Mesh();

  std::vector<Vertex> vertices_;
  std::vector<idx> indices_;
  Material material_; //TODO: make this through an shared pointer

  Mesh(const Mesh&) = delete;
  Mesh& operator=(const Mesh&) = delete;

  Mesh(Mesh&& other) noexcept;
  Mesh& operator=(Mesh&& other) noexcept;

  void draw(GLuint shader_program) const;

  friend std::ostream& operator<<(std::ostream& os, const Mesh& mesh);

private:
  uint VAO_=0;
  uint VBO_=0;
  uint EBO_=0;
  void setupMesh();
};
