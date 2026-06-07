//
// Created by Volodymyr Avvakumov on 25.05.2026.
//
#include "Mesh.h"

#include "Logger.h"
#include "glm/geometric.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <algorithm>
#include <limits>
#include <unordered_set>

namespace {
    struct MeshVertexStats {
        glm::vec2 min_tex_coord{std::numeric_limits<float>::max()};
        glm::vec2 max_tex_coord{std::numeric_limits<float>::lowest()};
        float min_normal_length = std::numeric_limits<float>::max();
        float max_normal_length = 0.0f;
        std::size_t zero_normal_count = 0;
    };

    MeshVertexStats collectVertexStats(const std::vector<Vertex>& vertices) {
        MeshVertexStats stats;

        for (const Vertex& vertex : vertices) {
            stats.min_tex_coord.x = std::min(stats.min_tex_coord.x, vertex.tex_coord_.x);
            stats.min_tex_coord.y = std::min(stats.min_tex_coord.y, vertex.tex_coord_.y);
            stats.max_tex_coord.x = std::max(stats.max_tex_coord.x, vertex.tex_coord_.x);
            stats.max_tex_coord.y = std::max(stats.max_tex_coord.y, vertex.tex_coord_.y);

            const float normal_length = glm::length(vertex.normal_);
            stats.min_normal_length = std::min(stats.min_normal_length, normal_length);
            stats.max_normal_length = std::max(stats.max_normal_length, normal_length);
            if (normal_length < 0.0001f) {
                ++stats.zero_normal_count;
            }
        }

        if (vertices.empty()) {
            stats.min_tex_coord = glm::vec2(0.0f);
            stats.max_tex_coord = glm::vec2(0.0f);
            stats.min_normal_length = 0.0f;
        }

        return stats;
    }

    void logMissingUniform(const char* name, const GLint location) {
        if (location == -1) {
            LOG_WARN("Mesh shader uniform is missing or optimized out: ", name);
        }
    }
} // namespace

std::ostream& operator<<(std::ostream& os, const Vertex& vertex) {
    os << "Vertex{position=(" << vertex.position_.x << ", " << vertex.position_.y << ", " << vertex.position_.z
       << "), normal=(" << vertex.normal_.x << ", " << vertex.normal_.y << ", " << vertex.normal_.z << "), tex_coord=("
       << vertex.tex_coord_.x << ", " << vertex.tex_coord_.y << ")}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Mesh& mesh) {
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

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<idx>& indices, Material material)
    : vertices_(vertices)
    , indices_(indices)
    , material_(std::move(material)) {
    LOG_INFO("Constructing mesh: vertices=", vertices_.size(), ", indices=", indices_.size());
    setupMesh();
}

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<idx>& indices,
           Material material,
           const bool has_tex_coords)
    : vertices_(vertices)
    , indices_(indices)
    , material_(std::move(material))
    , has_tex_coords_(has_tex_coords) {
    LOG_INFO("Constructing mesh: vertices=", vertices_.size(), ", indices=", indices_.size());
    setupMesh();
}

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<idx>& indices,
           Material material,
           const bool has_tex_coords,
           std::shared_ptr<Texture> diffuse_texture)
    : vertices_(vertices)
    , indices_(indices)
    , material_(std::move(material))
    , diffuse_texture_(std::move(diffuse_texture))
    , has_tex_coords_(has_tex_coords) {
    LOG_INFO("Constructing mesh: vertices=", vertices_.size(), ", indices=", indices_.size());
    setupMesh();
}

Mesh::~Mesh() {
    LOG_DEBUG("Destroying mesh: VAO=", VAO_, ", VBO=", VBO_, ", EBO=", EBO_);

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
    : vertices_(std::move(other.vertices_))
    , indices_(std::move(other.indices_))
    , material_(std::move(other.material_))
    , diffuse_texture_(std::move(other.diffuse_texture_))
    , use_texture_(other.use_texture_)
    , has_tex_coords_(other.has_tex_coords_)
    , VAO_(other.VAO_)
    , VBO_(other.VBO_)
    , EBO_(other.EBO_) {
    other.VAO_ = 0;
    other.VBO_ = 0;
    other.EBO_ = 0;
}


Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    LOG_DEBUG("Move-assigning mesh: current VAO=", VAO_, ", other VAO=", other.VAO_);

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
    diffuse_texture_ = std::move(other.diffuse_texture_);

    use_texture_ = other.use_texture_;
    has_tex_coords_ = other.has_tex_coords_;

    VAO_ = other.VAO_;
    VBO_ = other.VBO_;
    EBO_ = other.EBO_;

    return *this;
}

void Mesh::setupMesh() {
    LOG_DEBUG("Setting up mesh buffers: vertices=", vertices_.size(), ", indices=", indices_.size());

    constexpr GLuint position_location = 0;
    constexpr GLuint normal_location = 1;
    constexpr GLuint tex_coord_location = 2;
    constexpr GLint position_components = 3;
    constexpr GLint normal_components = 3;
    constexpr GLint tex_coord_components = 2;

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);

    LOG_DEBUG("Generated mesh buffers: VAO=", VAO_, ", VBO=", VBO_, ", EBO=", EBO_);

    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(
        GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices_.size() * sizeof(Vertex)), vertices_.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indices_.size() * sizeof(idx)),
                 indices_.data(),
                 GL_STATIC_DRAW);

    // layout(location = 0) vec3 position
    glVertexAttribPointer(position_location,
                          position_components,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, position_)));
    glEnableVertexAttribArray(position_location);

    // layout(location = 1) vec3 normal
    glVertexAttribPointer(normal_location,
                          normal_components,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, normal_)));
    glEnableVertexAttribArray(normal_location);

    // layout(location = 2) vec2 texCoord
    glVertexAttribPointer(tex_coord_location,
                          tex_coord_components,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, tex_coord_)));
    glEnableVertexAttribArray(tex_coord_location);

    glBindVertexArray(0);
    LOG_INFO("Mesh setup completed: VAO=", VAO_);
}

void Mesh::draw(GLuint shader_program) const {
    LOG_TRACE("Drawing mesh: VAO=", VAO_, ", indices=", indices_.size(), ", shader_program=", shader_program);

    const GLint diffuse_location = glGetUniformLocation(shader_program, "material.diffuse_color");
    const GLint specular_location = glGetUniformLocation(shader_program, "material.specular_color");
    const GLint ambient_location = glGetUniformLocation(shader_program, "material.ambient_color");
    const GLint shininess_location = glGetUniformLocation(shader_program, "material.shininess");
    const GLint use_texture_location = glGetUniformLocation(shader_program, "use_texture");
    const GLint diffuse_texture_location = glGetUniformLocation(shader_program, "diffuse_texture");

    static std::unordered_set<GLuint> logged_vaos;
    if (logged_vaos.insert(VAO_).second) {
        const MeshVertexStats stats = collectVertexStats(vertices_);
        LOG_INFO("Mesh draw diagnostics: VAO=",
                 VAO_,
                 ", vertices=",
                 vertices_.size(),
                 ", indices=",
                 indices_.size(),
                 ", has_tex_coords=",
                 has_tex_coords_,
                 ", use_texture_flag=",
                 use_texture_,
                 ", diffuse_texture_id=",
                 diffuse_texture_ != nullptr ? diffuse_texture_->getId() : 0,
                 ", texture_enabled=",
                 isTextureEnabled(),
                 ", tex_coord_min=(",
                 stats.min_tex_coord.x,
                 ", ",
                 stats.min_tex_coord.y,
                 "), tex_coord_max=(",
                 stats.max_tex_coord.x,
                 ", ",
                 stats.max_tex_coord.y,
                 "), normal_length_min=",
                 stats.min_normal_length,
                 ", normal_length_max=",
                 stats.max_normal_length,
                 ", zero_normals=",
                 stats.zero_normal_count,
                 ", material=",
                 material_);

        logMissingUniform("material.diffuse_color", diffuse_location);
        logMissingUniform("material.specular_color", specular_location);
        logMissingUniform("material.ambient_color", ambient_location);
        logMissingUniform("material.shininess", shininess_location);
        logMissingUniform("use_texture", use_texture_location);
        logMissingUniform("diffuse_texture", diffuse_texture_location);
    }

    glUniform3fv(diffuse_location, 1, glm::value_ptr(material_.diffuse_color));
    glUniform3fv(specular_location, 1, glm::value_ptr(material_.specular_color));
    glUniform3fv(ambient_location, 1, glm::value_ptr(material_.ambient_color));
    glUniform1f(shininess_location, material_.shininess);

    const bool should_use_texture = isTextureEnabled();
    glUniform1i(use_texture_location, should_use_texture);

    if (should_use_texture) {
        constexpr GLuint diffuse_texture_unit = 0;
        diffuse_texture_->bind(diffuse_texture_unit);
        glUniform1i(diffuse_texture_location, diffuse_texture_unit);
    }

    glBindVertexArray(VAO_);

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
}

bool Mesh::isTextureEnabled() const {
    return use_texture_ && has_tex_coords_ && diffuse_texture_ != nullptr;
}

void Mesh::enableTexture() {
    LOG_DEBUG("Enabling texture for mesh: VAO=", VAO_);
    use_texture_ = true;
}

void Mesh::disableTexture() {
    LOG_DEBUG("Disabling texture for mesh: VAO=", VAO_);
    use_texture_ = false;
}

void Mesh::setDiffuseTexture(std::shared_ptr<Texture> texture) {
    LOG_INFO("Setting diffuse texture for mesh: VAO=",
             VAO_,
             ", texture_id=",
             texture != nullptr ? texture->getId() : 0,
             ", has_tex_coords=",
             has_tex_coords_);
    diffuse_texture_ = std::move(texture);
}
