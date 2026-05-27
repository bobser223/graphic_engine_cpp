//
// Created by Volodymyr Avvakumov on 27.05.2026.
//

#ifndef ENGINE_TEXTURE_H
#define ENGINE_TEXTURE_H
#include <string>
#include <utility>

#include "Logger.h"
#include "stb_image.h"
#include "glad/glad.h"


enum class TextureType {
    Diffuse,
    Specular,
    Normal,
    Unknown
};




class Texture {


public:
    explicit Texture(
        std::string  path,
        const TextureType type = TextureType::Diffuse,
        const bool flip_vertically = true
    ) : type_(type), path_(std::move(path)) {
            id_ = createTextureFromFile(path_, flip_vertically);

            if (id_ == 0) {
                throw std::runtime_error("Failed to create texture: " + path_);
            }
    }

    ~Texture() {
        if (id_ != 0) {
            glDeleteTextures(1, &id_);
        }
    }

    Texture(Texture&& other) noexcept {
        id_ = other.id_;
        target_ = other.target_;
        path_ = std::move(other.path_);

        other.id_ = 0;
        other.target_ = GL_TEXTURE_2D;
    }

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture& operator=(Texture&& other) noexcept {
        if (this != &other) {
            if (id_ != 0) {
                glDeleteTextures(1, &id_);
            }

            id_ = other.id_;
            target_ = other.target_;
            path_ = std::move(other.path_);

            other.id_ = 0;
        }

        return *this;
    }



    void bind(const GLuint unit = 0) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(target_, id_);
    }

    void unbind() const {
        glBindTexture(target_, 0);
    }

    [[nodiscard]] GLuint getId() const {
        return id_;
    }

    [[nodiscard]] GLenum getTarget() const {
        return target_;
    }

private:
    GLuint id_ = 0;
    GLenum target_ = GL_TEXTURE_2D;
    TextureType type_ = TextureType::Unknown;

    std::string path_;

    static GLuint createTextureFromFile(
    const std::filesystem::path& texture_path,
    bool flip_vertically
        ) {
        LOG_INFO("Creating texture from file: ", texture_path, ", flip_vertically=", flip_vertically);

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
            LOG_ERROR("Failed to load texture: ", texture_path);

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
            LOG_ERROR("Unsupported texture channel count: ", channels, " in file: ", texture_path);

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

        LOG_INFO(
            "Texture created successfully: ", texture_path,
            " id=", texture_id,
            " size=", width, "x", height,
            " channels=", channels
        );

        return texture_id;
    }


};


#endif //ENGINE_TEXTURE_H
