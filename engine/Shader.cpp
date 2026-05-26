//
// Created by Volodymyr Avvakumov on 26.05.2026.
//

#include "Shader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "Logger.h"
#include "glm/gtc/type_ptr.hpp"

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path) {
    const GLuint vertex_shader = createShaderFromFile(GL_VERTEX_SHADER, vertex_path);
    const GLuint fragment_shader = createShaderFromFile(GL_FRAGMENT_SHADER, fragment_path);

    program_id_ = createProgram(vertex_shader, fragment_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

Shader::~Shader() {
    if (program_id_ != 0) {
        glDeleteProgram(program_id_);
    }
}

Shader::Shader(Shader&& other) noexcept
    : program_id_(std::exchange(other.program_id_, 0)) {
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (program_id_ != 0) {
            glDeleteProgram(program_id_);
        }

        program_id_ = std::exchange(other.program_id_, 0);
    }

    return *this;
}

void Shader::use() const {
    glUseProgram(program_id_);
}

GLuint Shader::getProgramId() const {
    return program_id_;
}

void Shader::setUniform(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniform(const std::string& name, const glm::mat3& value) const {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniform(const std::string& name, const glm::mat2& value) const {
    glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniform(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setUniform(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setUniform(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setUniform(const std::string& name, const float value) const {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setUniform(const std::string& name, const int value) const {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniform(const std::string& name, const bool value) const {
    glUniform1i(getUniformLocation(name), value);
}

GLint Shader::getUniformLocation(const std::string& name) const {
    const GLint location = glGetUniformLocation(program_id_, name.c_str());

    if (location == -1) {
        LOG_DEBUG("Uniform not found or optimized out: ", name);
    }

    return location;
}

std::string Shader::readShaderFromFile(const std::filesystem::path& path) {
    LOG_DEBUG("Reading shader file: ", path);

    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_ERROR("Cannot open shader file: ", path);
        throw std::runtime_error("Cannot open shader file: " + path.string());
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    LOG_INFO("Shader file read successfully: ", path, " (", buffer.str().size(), " bytes)");

    return buffer.str();
}

GLuint Shader::createShaderFromFile(const GLenum shader_type, const std::filesystem::path& path) {
    LOG_INFO("Creating shader from file: ", path);

    const std::string shader_code = readShaderFromFile(path);
    const char* source = shader_code.c_str();

    const GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

        std::string info_log(static_cast<size_t>(log_length), '\0');
        glGetShaderInfoLog(shader, log_length, nullptr, info_log.data());

        LOG_ERROR("Shader compilation failed for ", path, ": ", info_log);

        glDeleteShader(shader);

        throw std::runtime_error(
            "Shader compilation failed: " + path.string() + "\n" + info_log
        );
    }

    LOG_INFO("Shader compiled successfully: ", path, " id=", shader);
    return shader;
}

GLuint Shader::createProgram(const GLuint vertex_shader, const GLuint fragment_shader) {
    const GLuint shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    GLint success = 0;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        GLint log_length = 0;
        glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &log_length);

        std::string info_log(static_cast<size_t>(log_length), '\0');
        glGetProgramInfoLog(shader_program, log_length, nullptr, info_log.data());

        LOG_ERROR("Shader program linking failed: ", info_log);
        glDeleteProgram(shader_program);

        throw std::runtime_error("Shader program linking failed: " + info_log);
    }

    LOG_INFO("Shader program linked successfully: id=", shader_program);
    return shader_program;
}
