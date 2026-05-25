//
// Created by Volodymyr Avvakumov on 11.04.2026.
//
#include "creator.h"
#include <string>
#include "reader.h"

GLuint createShaderFromFile(GLenum shader_type, const std::filesystem::path& path) {
    std::string shaderCode = readShaderFromFile(path);
    const char* src = shaderCode.c_str();

    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    return shader;
}