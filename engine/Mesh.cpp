//
// Created by Volodymyr Avvakumov on 25.05.2026.
//
#include "Mesh.h"

#include "glm/gtc/type_ptr.hpp"


Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<idx>& indices,
    Material material): vertices_(vertices), indices_(indices), material_(std::move(material))
{
    setupMesh();
}

Mesh::~Mesh() {
    if (VAO_ != 0) {
        glDeleteVertexArrays(1, &VAO_);
    }
    if (VBO_ != 0) {
        glDeleteBuffers(1, &VBO_);
    }
    if (EBO_ != 0) {
        glDeleteBuffers(1, &EBO_);
    }
}


Mesh::Mesh(Mesh&& other) noexcept
    : vertices_(std::move(other.vertices_)),
    indices_(std::move(other.indices_)),
    material_(std::move(other.material_)),
    VAO_(other.VAO_),
    VBO_(other.VBO_),
    EBO_(other.EBO_)
{
    other.VAO_ = 0;
    other.VBO_ = 0;
    other.EBO_ = 0;
}


Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        return *this;
    }

    if (EBO_ != 0) {
        glDeleteBuffers(1, &EBO_);
    }

    if (VBO_ != 0) {
        glDeleteBuffers(1, &VBO_);
    }

    if (VAO_ != 0) {
        glDeleteVertexArrays(1, &VAO_);
    }

    vertices_ = std::move(other.vertices_);
    indices_ = std::move(other.indices_);
    material_ = std::move(other.material_);

    VAO_ = other.VAO_;
    VBO_ = other.VBO_;
    EBO_ = other.EBO_;

    other.VAO_ = 0;
    other.VBO_ = 0;
    other.EBO_ = 0;

    return *this;
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);

    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices_.size() * sizeof(Vertex)),
        vertices_.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(indices_.size() * sizeof(idx)),
        indices_.data(),
        GL_STATIC_DRAW
    );

    // layout(location = 0) vec3 position
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, position_))
    );
    glEnableVertexAttribArray(0);

    // layout(location = 1) vec3 normal
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, normal_))
    );
    glEnableVertexAttribArray(1);

    // layout(location = 2) vec2 texCoord
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, tex_coord_))
    );
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::draw(GLuint shader_program) const {
    GLint diffuse_location = glGetUniformLocation(shader_program, "material.diffuse_color");
    GLint specular_location = glGetUniformLocation(shader_program, "material.specular_color");
    GLint ambient_location = glGetUniformLocation(shader_program, "material.ambient_color");
    GLint shininess_location = glGetUniformLocation(shader_program, "material.shininess");

    glUniform3fv(diffuse_location, 1, glm::value_ptr(material_.diffuse_color));
    glUniform3fv(specular_location, 1, glm::value_ptr(material_.specular_color));
    glUniform3fv(ambient_location, 1, glm::value_ptr(material_.ambient_color));
    glUniform1f(shininess_location, material_.shininess);

    glBindVertexArray(VAO_);

    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(indices_.size()),
        GL_UNSIGNED_INT,
        nullptr
    );

    glBindVertexArray(0);
}
