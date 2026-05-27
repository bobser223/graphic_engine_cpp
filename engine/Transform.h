#ifndef ENGINE_TRANSFORM_H
#define ENGINE_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform {
public:
    Transform() = default;

    Transform(
        const glm::vec3& position,
        const glm::quat& rotation,
        const glm::vec3& scale
    )
        : position_(position),
          rotation_(rotation),
          scale_(scale) {}

    [[nodiscard]] glm::mat4 getMatrix() const {
        auto result = glm::mat4(1.0f);

        result = glm::translate(result, position_);
        result *= glm::toMat4(rotation_);
        result = glm::scale(result, scale_);

        return result;
    }

    void setPosition(const glm::vec3& position) {
        position_ = position;
    }

    void setScale(const glm::vec3& scale) {
        scale_ = scale;
    }

    void setRotation(const glm::quat& rotation) {
        rotation_ = glm::normalize(rotation);
    }

    void setRotationEulerDegrees(const glm::vec3& degrees) {
        glm::vec3 radians = glm::radians(degrees);
        rotation_ = glm::quat(radians);
    }

    void setRotationAxisAngleDegrees(const float degrees, const glm::vec3& axis) {
        rotation_ = glm::angleAxis(
            glm::radians(degrees),
            glm::normalize(axis)
        );
    }

    void rotateAxisAngleDegrees(const float degrees, const glm::vec3& axis) {
        const glm::quat delta = glm::angleAxis(
            glm::radians(degrees),
            glm::normalize(axis)
        );

        rotation_ = glm::normalize(delta * rotation_);
    }

    [[nodiscard]] const glm::vec3& getPosition() const {
        return position_;
    }

    [[nodiscard]] const glm::vec3& getScale() const {
        return scale_;
    }

    [[nodiscard]] const glm::quat& getRotation() const {
        return rotation_;
    }

public:
    glm::vec3 position_ = glm::vec3(0.0f);
    glm::quat rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale_ = glm::vec3(1.0f);
};

#endif // ENGINE_TRANSFORM_H