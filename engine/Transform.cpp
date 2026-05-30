//
// Created by Volodymyr Avvakumov on 27.05.2026.
//

#include "Transform.h"

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

#include "Logger.h"

namespace {
constexpr float same_direction_threshold = 0.9999f;
constexpr float opposite_direction_threshold = -0.9999f;
constexpr float fallback_axis_length_threshold = 0.0001f;
constexpr float half_turn_degrees = 180.0f;
}

Transform::Transform(
    const glm::vec3& position,
    const glm::quat& rotation,
    const glm::vec3& scale
)
    : position_(position),
      rotation_(glm::normalize(rotation)),
      scale_(scale) {
    LOG_DEBUG("Transform created: position=(", position_.x, ", ", position_.y, ", ", position_.z, ")");
}

glm::mat4 Transform::getMatrix() const {
    glm::mat4 result(1.0f);

    result = glm::translate(result, position_);
    result *= quaternionToMat4(rotation_);
    result = glm::scale(result, scale_);

    return result;
}

void Transform::setPosition(const glm::vec3& position) {
    position_ = position;
    LOG_TRACE("Transform position set: (", position_.x, ", ", position_.y, ", ", position_.z, ")");
}

void Transform::setScale(const glm::vec3& scale) {
    scale_ = scale;
    LOG_TRACE("Transform scale set: (", scale_.x, ", ", scale_.y, ", ", scale_.z, ")");
}

void Transform::setRotation(const glm::quat& rotation) {
    rotation_ = glm::normalize(rotation);
    LOG_TRACE("Transform rotation set");
}

void Transform::setRotationEulerDegrees(const glm::vec3& degrees) {
    const glm::vec3 radians = glm::radians(degrees);
    rotation_ = glm::quat(radians);
    LOG_TRACE("Transform rotation set from Euler degrees: (", degrees.x, ", ", degrees.y, ", ", degrees.z, ")");
}

void Transform::setRotationAxisAngleDegrees(const float degrees, const glm::vec3& axis) {
    rotation_ = glm::angleAxis(
        glm::radians(degrees),
        glm::normalize(axis)
    );
    LOG_TRACE("Transform rotation set from axis-angle: degrees=", degrees);
}

void Transform::rotateAxisAngleDegrees(const float degrees, const glm::vec3& axis) {
    const glm::quat delta = glm::angleAxis(
        glm::radians(degrees),
        glm::normalize(axis)
    );

    rotation_ = glm::normalize(delta * rotation_);
    LOG_TRACE("Transform rotated by axis-angle: degrees=", degrees);
}

const glm::vec3& Transform::getPosition() const {
    return position_;
}

const glm::vec3& Transform::getScale() const {
    return scale_;
}

const glm::quat& Transform::getRotation() const {
    return rotation_;
}

void Transform::setRotationFromTo(glm::vec3 from, glm::vec3 to) {
    from = glm::normalize(from);
    to = glm::normalize(to);

    const float cos_theta = glm::dot(from, to);

    if (cos_theta > same_direction_threshold) {
        rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        LOG_TRACE("Transform rotation from-to resolved as identity");
        return;
    }

    if (cos_theta < opposite_direction_threshold) {
        glm::vec3 axis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), from);

        if (glm::length(axis) < fallback_axis_length_threshold) {
            axis = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), from);
        }

        axis = glm::normalize(axis);
        rotation_ = glm::angleAxis(glm::radians(half_turn_degrees), axis);
        LOG_TRACE("Transform rotation from-to resolved as half-turn");
        return;
    }

    const glm::vec3 axis = glm::normalize(glm::cross(from, to));
    const float angle = std::acos(cos_theta);

    rotation_ = glm::angleAxis(angle, axis);
    LOG_TRACE("Transform rotation from-to set: angle=", angle);
}

void Transform::translate(const glm::vec3& delta) {
    position_ += delta;
    LOG_TRACE("Transform translated: delta=(", delta.x, ", ", delta.y, ", ", delta.z, ")");
}

void Transform::translateLocal(const glm::vec3& local_delta) {
    position_ += getRight() * local_delta.x;
    position_ += getUp() * local_delta.y;
    position_ += getForward() * local_delta.z;
    LOG_TRACE("Transform translated locally: delta=(", local_delta.x, ", ", local_delta.y, ", ", local_delta.z, ")");
}

glm::vec3 Transform::getForward() const {
    return glm::normalize(rotation_ * glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 Transform::getRight() const {
    return glm::normalize(rotation_ * glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 Transform::getUp() const {
    return glm::normalize(rotation_ * glm::vec3(0.0f, 1.0f, 0.0f));
}

void Transform::moveForward(const float distance) {
    position_ += getForward() * distance;
    LOG_TRACE("Transform moved forward: distance=", distance);
}

void Transform::moveBack(const float distance) {
    position_ -= getForward() * distance;
    LOG_TRACE("Transform moved back: distance=", distance);
}

void Transform::moveRight(const float distance) {
    position_ += getRight() * distance;
    LOG_TRACE("Transform moved right: distance=", distance);
}

void Transform::moveLeft(const float distance) {
    position_ -= getRight() * distance;
    LOG_TRACE("Transform moved left: distance=", distance);
}

void Transform::moveUp(const float distance) {
    position_ += getUp() * distance;
    LOG_TRACE("Transform moved up: distance=", distance);
}

void Transform::moveDown(const float distance) {
    position_ -= getUp() * distance;
    LOG_TRACE("Transform moved down: distance=", distance);
}

void Transform::rotateWorldAxisAngleDegrees(const float degrees, const glm::vec3& axis) {
    const glm::quat delta = glm::angleAxis(
        glm::radians(degrees),
        glm::normalize(axis)
    );

    rotation_ = glm::normalize(delta * rotation_);
    LOG_TRACE("Transform rotated around world axis: degrees=", degrees);
}

void Transform::rotateLocalAxisAngleDegrees(const float degrees, const glm::vec3& axis) {
    const glm::quat delta = glm::angleAxis(
        glm::radians(degrees),
        glm::normalize(axis)
    );

    rotation_ = glm::normalize(rotation_ * delta);
    LOG_TRACE("Transform rotated around local axis: degrees=", degrees);
}

glm::mat4 Transform::quaternionToMat4(glm::quat q) {
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

    glm::mat4 matrix(1.0f);

    matrix[0][0] = 1.0f - yy - zz;
    matrix[0][1] = xy + wz;
    matrix[0][2] = xz - wy;
    matrix[0][3] = 0.0f;

    matrix[1][0] = xy - wz;
    matrix[1][1] = 1.0f - xx - zz;
    matrix[1][2] = yz + wx;
    matrix[1][3] = 0.0f;

    matrix[2][0] = xz + wy;
    matrix[2][1] = yz - wx;
    matrix[2][2] = 1.0f - xx - yy;
    matrix[2][3] = 0.0f;

    matrix[3][0] = 0.0f;
    matrix[3][1] = 0.0f;
    matrix[3][2] = 0.0f;
    matrix[3][3] = 1.0f;

    return matrix;
}
