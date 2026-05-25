//
// Created by Volodymyr Avvakumov on 03.05.2026.
//

#ifndef COMPUTERGRAPHICS_TEXTURE_CREATOR_H
#define COMPUTERGRAPHICS_TEXTURE_CREATOR_H

#include <filesystem>
#include <glad/glad.h>

GLuint createTextureFromFile(
    const std::filesystem::path& texture_path,
    bool flip_vertically = true
);

#endif //COMPUTERGRAPHICS_TEXTURE_CREATOR_H
