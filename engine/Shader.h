//
// Created by Volodymyr Avvakumov on 26.05.2026.
//

#ifndef ENGINE_SHADER_H
#define ENGINE_SHADER_H

#include "glad/glad.h"
#include "glm/glm.hpp"

#include <filesystem>
#include <string>


class Shader {
public:
    Shader(const std::string& vertex_path, const std::string& fragment_path);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void use() const;
    GLuint getProgramId() const;

    void setUniform(const std::string& name, const glm::mat4& value) const;
    void setUniform(const std::string& name, const glm::mat3& value) const;
    void setUniform(const std::string& name, const glm::mat2& value) const;

    void setUniform(const std::string& name, const glm::vec2& value) const;
    void setUniform(const std::string& name, const glm::vec3& value) const;
    void setUniform(const std::string& name, const glm::vec4& value) const;

    void setUniform(const std::string& name, float value) const;
    void setUniform(const std::string& name, int value) const;
    void setUniform(const std::string& name, bool value) const;

    GLint getUniformLocation(const std::string& name) const;

private:
    static std::string readShaderFromFile(const std::filesystem::path& path);
    static GLuint createShaderFromFile(GLenum shader_type, const std::filesystem::path& path);
    static GLuint createProgram(GLuint vertex_shader, GLuint fragment_shader);

    GLuint program_id_ = 0;
};

#endif // ENGINE_SHADER_H
