//
// Created by Volodymyr Avvakumov on 27.05.2026.
//

#include "Texture.h"

#include "Logger.h"
#include "stb_image.h"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

namespace {
    void logTexturePixelStats(const std::filesystem::path& texture_path,
                              const unsigned char* data,
                              const int width,
                              const int height,
                              const int channels) {
        const std::size_t pixel_count = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
        if (data == nullptr || pixel_count == 0 || channels <= 0) {
            LOG_WARN("Texture pixel stats unavailable: ", texture_path);
            return;
        }

        unsigned int min_r = std::numeric_limits<unsigned int>::max();
        unsigned int min_g = std::numeric_limits<unsigned int>::max();
        unsigned int min_b = std::numeric_limits<unsigned int>::max();
        unsigned int max_r = 0;
        unsigned int max_g = 0;
        unsigned int max_b = 0;
        unsigned long long sum_r = 0;
        unsigned long long sum_g = 0;
        unsigned long long sum_b = 0;
        std::size_t black_pixels = 0;

        unsigned int min_a = std::numeric_limits<unsigned int>::max();
        unsigned int max_a = 0;
        std::size_t transparent_pixels = 0;

        for (std::size_t i = 0; i < pixel_count; ++i) {
            const std::size_t offset = i * static_cast<std::size_t>(channels);
            const unsigned int r = data[offset];
            const unsigned int g = channels == 1 ? r : data[offset + 1];
            const unsigned int b = channels == 1 ? r : data[offset + 2];

            min_r = std::min(min_r, r);
            min_g = std::min(min_g, g);
            min_b = std::min(min_b, b);
            max_r = std::max(max_r, r);
            max_g = std::max(max_g, g);
            max_b = std::max(max_b, b);
            sum_r += r;
            sum_g += g;
            sum_b += b;

            if (r <= 4 && g <= 4 && b <= 4) {
                ++black_pixels;
            }

            if (channels == 4) {
                const unsigned int a = data[offset + 3];
                min_a = std::min(min_a, a);
                max_a = std::max(max_a, a);
                if (a == 0) {
                    ++transparent_pixels;
                }
            }
        }

        LOG_INFO("Texture pixel stats: path=",
                 texture_path,
                 ", pixels=",
                 pixel_count,
                 ", rgb_min=(",
                 min_r,
                 ", ",
                 min_g,
                 ", ",
                 min_b,
                 "), rgb_max=(",
                 max_r,
                 ", ",
                 max_g,
                 ", ",
                 max_b,
                 "), rgb_avg=(",
                 static_cast<double>(sum_r) / static_cast<double>(pixel_count),
                 ", ",
                 static_cast<double>(sum_g) / static_cast<double>(pixel_count),
                 ", ",
                 static_cast<double>(sum_b) / static_cast<double>(pixel_count),
                 "), black_pixels=",
                 black_pixels);

        if (channels == 4) {
            LOG_INFO("Texture alpha stats: path=",
                     texture_path,
                     ", alpha_min=",
                     min_a,
                     ", alpha_max=",
                     max_a,
                     ", transparent_pixels=",
                     transparent_pixels);
        }
    }
} // namespace

Texture::Texture(std::string path, const TextureType type, const bool flip_vertically)
    : type_(type)
    , path_(std::move(path)) {
    LOG_INFO("Creating Texture object: path=",
             path_,
             ", type=",
             static_cast<int>(type_),
             ", flip_vertically=",
             flip_vertically);

    id_ = createTextureFromFile(path_, flip_vertically);

    if (id_ == 0) {
        LOG_ERROR("Texture object creation failed: path=", path_);
        throw std::runtime_error("Failed to create texture: " + path_);
    }

    LOG_INFO("Texture object created: id=", id_, ", path=", path_);
}

Texture::~Texture() {
    if (id_ != 0) {
        LOG_DEBUG("Deleting texture: id=", id_, ", path=", path_);
        glDeleteTextures(1, &id_);
    }
}

Texture::Texture(Texture&& other) noexcept
    : id_(other.id_)
    , target_(other.target_)
    , type_(other.type_)
    , path_(std::move(other.path_)) {
    LOG_DEBUG("Move-constructing texture: id=", id_, ", path=", path_);

    other.id_ = 0;
    other.target_ = GL_TEXTURE_2D;
    other.type_ = TextureType::Unknown;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        if (id_ != 0) {
            LOG_DEBUG("Deleting texture before move assignment: id=", id_, ", path=", path_);
            glDeleteTextures(1, &id_);
        }

        id_ = other.id_;
        target_ = other.target_;
        type_ = other.type_;
        path_ = std::move(other.path_);

        other.id_ = 0;
        other.target_ = GL_TEXTURE_2D;
        other.type_ = TextureType::Unknown;

        LOG_DEBUG("Move-assigning texture: id=", id_, ", path=", path_);
    }

    return *this;
}

void Texture::bind(const GLuint unit) const {
    LOG_TRACE("Binding texture: id=", id_, ", target=", target_, ", unit=", unit);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(target_, id_);
}

void Texture::unbind() const {
    LOG_TRACE("Unbinding texture target: ", target_);
    glBindTexture(target_, 0);
}

GLuint Texture::getId() const {
    return id_;
}

GLenum Texture::getTarget() const {
    return target_;
}

GLuint Texture::createTextureFromFile(const std::filesystem::path& texture_path, const bool flip_vertically) {
    LOG_INFO("Creating texture from file: ", texture_path, ", flip_vertically=", flip_vertically);

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(flip_vertically);

    int width = 0;
    int height = 0;
    int channels = 0;

    unsigned char* data = stbi_load(texture_path.string().c_str(), &width, &height, &channels, 0);

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

    logTexturePixelStats(texture_path, data, width, height, channels);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    LOG_INFO("Texture created successfully: ",
             texture_path,
             " id=",
             texture_id,
             " size=",
             width,
             "x",
             height,
             " channels=",
             channels);

    return texture_id;
}
