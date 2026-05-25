//
// Created by Volodymyr Avvakumov on 11.04.2026.
//

#ifndef COMPUTERGRAPHICS_CREATOR_H
#define COMPUTERGRAPHICS_CREATOR_H

#include "glad/glad.h"
#include <filesystem>

GLuint createShaderFromFile(GLenum shader_type, const std::filesystem::path& path);

#endif //COMPUTERGRAPHICS_CREATOR_H
