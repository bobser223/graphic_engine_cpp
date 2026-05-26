//
// Created by Volodymyr Avvakumov on 11.04.2026.
//
#include "creator.h"
#include <string>
#include "reader.h"
#include "Logger.h"

GLuint createShaderFromFile(GLenum shader_type, const std::filesystem::path& path) {
    LOG_INFO("Creating shader from file: ", path);

    std::string shaderCode = readShaderFromFile(path);
    const char* src = shaderCode.c_str();

    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

        std::string info_log(static_cast<size_t>(log_length), '\0');
        glGetShaderInfoLog(shader, log_length, nullptr, info_log.data());

        LOG_ERROR("Shader compilation failed for ", path, ": ", info_log);
    } else {
        LOG_INFO("Shader compiled successfully: ", path, " id=", shader);
    }

    return shader;
}
