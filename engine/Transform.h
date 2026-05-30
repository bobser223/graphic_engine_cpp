#ifndef ENGINE_TRANSFORM_H
#define ENGINE_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// #include <glm/gtx/quaternion.hpp>

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

        result *= quaternionToMat4(rotation_);

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

    void setRotationFromTo(glm::vec3 from, glm::vec3 to) {
        from = glm::normalize(from);
        to = glm::normalize(to);

        float cos_theta = glm::dot(from, to);

        if (cos_theta > 0.9999f) {
            rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            return;
        }

        if (cos_theta < -0.9999f) {
            glm::vec3 axis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), from);

            if (glm::length(axis) < 0.0001f) {
                axis = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), from);
            }

            axis = glm::normalize(axis);
            rotation_ = glm::angleAxis(glm::radians(180.0f), axis);
            return;
        }

        glm::vec3 axis = glm::normalize(glm::cross(from, to));
        float angle = std::acos(cos_theta);

        rotation_ = glm::angleAxis(angle, axis);
    }

    void translate(const glm::vec3& delta) {
        position_ += delta;
    }

    void translateLocal(const glm::vec3& local_delta) {
        position_ += getRight()   * local_delta.x;
        position_ += getUp()      * local_delta.y;
        position_ += getForward() * local_delta.z;
    }

    [[nodiscard]] glm::vec3 getForward() const {
        return glm::normalize(rotation_ * glm::vec3(0.0f, 0.0f, -1.0f));
    }

    [[nodiscard]] glm::vec3 getRight() const {
        return glm::normalize(rotation_ * glm::vec3(1.0f, 0.0f, 0.0f));
    }

    [[nodiscard]] glm::vec3 getUp() const {
        return glm::normalize(rotation_ * glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void moveForward(float distance) {
        position_ += getForward() * distance;
    }

    void moveBack(float distance) {
        position_ -= getForward() * distance;
    }

    void moveRight(float distance) {
        position_ += getRight() * distance;
    }

    void moveLeft(float distance) {
        position_ -= getRight() * distance;
    }

    void moveUp(float distance) {
        position_ += getUp() * distance;
    }

    void moveDown(float distance) {
        position_ -= getUp() * distance;
    }

    void rotateWorldAxisAngleDegrees(float degrees, const glm::vec3& axis) {
        const glm::quat delta = glm::angleAxis(
            glm::radians(degrees),
            glm::normalize(axis)
        );

        rotation_ = glm::normalize(delta * rotation_);
    }

    void rotateLocalAxisAngleDegrees(float degrees, const glm::vec3& axis) {
        const glm::quat delta = glm::angleAxis(
            glm::radians(degrees),
            glm::normalize(axis)
        );

        rotation_ = glm::normalize(rotation_ * delta);
    }

    glm::vec3 position_ = glm::vec3(0.0f);
    glm::quat rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale_ = glm::vec3(1.0f);

    [[nodiscard]] static glm::mat4 quaternionToMat4(glm::quat q) {
        q = glm::normalize(q);

        const float w = q.w;
        const float x = q.x;
        const float y = q.y;
        const float z = q.z;

        const float x2 = x + x;
        const float y2 = y + y;
        const float z2 = z + z;

        const float xx = x * x2;
        const float yy = y * y2;
        const float zz = z * z2;

        const float xy = x * y2;
        const float xz = x * z2;
        const float yz = y * z2;

        const float wx = w * x2;
        const float wy = w * y2;
        const float wz = w * z2;

        glm::mat4 m(1.0f);

        m[0][0] = 1.0f - yy - zz;
        m[0][1] = xy + wz;
        m[0][2] = xz - wy;
        m[0][3] = 0.0f;

        m[1][0] = xy - wz;
        m[1][1] = 1.0f - xx - zz;
        m[1][2] = yz + wx;
        m[1][3] = 0.0f;

        m[2][0] = xz + wy;
        m[2][1] = yz - wx;
        m[2][2] = 1.0f - xx - yy;
        m[2][3] = 0.0f;

        m[3][0] = 0.0f;
        m[3][1] = 0.0f;
        m[3][2] = 0.0f;
        m[3][3] = 1.0f;

        return m;
    }


};

#endif // ENGINE_TRANSFORM_H