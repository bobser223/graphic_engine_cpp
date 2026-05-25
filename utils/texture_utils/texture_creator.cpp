//
// Created by Volodymyr Avvakumov on 03.05.2026.
//


#include "texture_creator.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint createTextureFromFile(
    const std::filesystem::path& texture_path,
    bool flip_vertically
) {
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // Поведінка, якщо texture coordinates виходять за [0, 1]
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Фільтрація при зменшенні/збільшенні текстури
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(flip_vertically);

    int width = 0;
    int height = 0;
    int channels = 0;

    unsigned char* data = stbi_load(
        texture_path.string().c_str(),
        &width,
        &height,
        &channels,
        0
    );

    if (!data) {
        std::cerr << "Failed to load texture: "
                  << texture_path
                  << std::endl;

        glDeleteTextures(1, &texture_id);
        return 0;
    }

    GLenum format;

    if (channels == 1) {
        format = GL_RED;
    } else if (channels == 3) {
        format = GL_RGB;
    } else if (channels == 4) {
        format = GL_RGBA;
    } else {
        std::cerr << "Unsupported texture channel count: "
                  << channels
                  << " in file: "
                  << texture_path
                  << std::endl;

        stbi_image_free(data);
        glDeleteTextures(1, &texture_id);
        return 0;
    }

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture_id;
}
