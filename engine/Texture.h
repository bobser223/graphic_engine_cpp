//
// Created by Volodymyr Avvakumov on 27.05.2026.
//

#ifndef ENGINE_TEXTURE_H
#define ENGINE_TEXTURE_H
#include "glad/glad.h"

#include <filesystem>
#include <string>


enum class TextureType { Diffuse, Specular, Normal, Unknown };



class Texture {


public:
    explicit Texture(std::string path, TextureType type = TextureType::Diffuse, bool flip_vertically = true);

    ~Texture();

    Texture(Texture&& other) noexcept;

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture& operator=(Texture&& other) noexcept;

    void bind(GLuint unit = 0) const;
    void unbind() const;

    [[nodiscard]] GLuint getId() const;
    [[nodiscard]] GLenum getTarget() const;

private:
    static GLuint createTextureFromFile(const std::filesystem::path& texture_path, bool flip_vertically);

    GLuint id_ = 0;
    GLenum target_ = GL_TEXTURE_2D;
    TextureType type_ = TextureType::Unknown;

    std::string path_;
};


#endif // ENGINE_TEXTURE_H
