//
// Created by Volodymyr Avvakumov on 26.05.2026.
//

#include "Camera.h"

#include <algorithm>
#include <cmath>

#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#include "Logger.h"

float Camera::getVelocity(const float delta_time) const {
    return delta_time * camera_speed_;
}

glm::vec3 Camera::getCameraRight() const {
    return glm::normalize(glm::cross(camera_front_, camera_up_));
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(camera_pos_, camera_pos_ + camera_front_, camera_up_);
}

void Camera::goToward(const float delta_time) {
    camera_pos_ += camera_front_ * getVelocity(delta_time);
    LOG_TRACE("Camera moved toward: dt=", delta_time, ", pos=(", camera_pos_.x, ", ", camera_pos_.y, ", ", camera_pos_.z, ")");
}

void Camera::goBack(const float delta_time) {
    camera_pos_ -= camera_front_ * getVelocity(delta_time);
    LOG_TRACE("Camera moved back: dt=", delta_time, ", pos=(", camera_pos_.x, ", ", camera_pos_.y, ", ", camera_pos_.z, ")");
}

void Camera::goRight(const float delta_time) {
    camera_pos_ += getCameraRight() * getVelocity(delta_time);
    LOG_TRACE("Camera moved right: dt=", delta_time, ", pos=(", camera_pos_.x, ", ", camera_pos_.y, ", ", camera_pos_.z, ")");
}

void Camera::goLeft(const float delta_time) {
    camera_pos_ -= getCameraRight() * getVelocity(delta_time);
    LOG_TRACE("Camera moved left: dt=", delta_time, ", pos=(", camera_pos_.x, ", ", camera_pos_.y, ", ", camera_pos_.z, ")");
}

void Camera::goUp(const float delta_time) {
    camera_pos_ += camera_up_ * getVelocity(delta_time);
    LOG_TRACE("Camera moved up: dt=", delta_time, ", pos=(", camera_pos_.x, ", ", camera_pos_.y, ", ", camera_pos_.z, ")");
}

void Camera::goDown(const float delta_time) {
    camera_pos_ -= camera_up_ * getVelocity(delta_time);
    LOG_TRACE("Camera moved down: dt=", delta_time, ", pos=(", camera_pos_.x, ", ", camera_pos_.y, ", ", camera_pos_.z, ")");
}

void Camera::rotateYawClockwiseByDegrees(const float degrees) {
    yaw_ += degrees;
    updateCameraFront();
    LOG_TRACE("Camera yaw rotated clockwise: degrees=", degrees, ", yaw=", yaw_);
}

void Camera::rotateYawClockwiseByTime(const float delta_time) {
    rotateYawClockwiseByDegrees(delta_time * rotation_speed_);
}

void Camera::rotateYawCounterClockwiseByDegrees(const float degrees) {
    yaw_ -= degrees;
    updateCameraFront();
    LOG_TRACE("Camera yaw rotated counter-clockwise: degrees=", degrees, ", yaw=", yaw_);
}

void Camera::rotateYawCounterClockwiseByTime(const float delta_time) {
    rotateYawCounterClockwiseByDegrees(delta_time * rotation_speed_);
}

void Camera::rotatePitchUpByDegrees(const float degrees) {
    pitch_ += degrees;
    clampPitch();
    updateCameraFront();
    LOG_TRACE("Camera pitch rotated up: degrees=", degrees, ", pitch=", pitch_);
}

void Camera::rotatePitchUpByTime(const float delta_time) {
    rotatePitchUpByDegrees(delta_time * rotation_speed_);
}

void Camera::rotatePitchDownByDegrees(const float degrees) {
    pitch_ -= degrees;
    clampPitch();
    updateCameraFront();
    LOG_TRACE("Camera pitch rotated down: degrees=", degrees, ", pitch=", pitch_);
}

void Camera::rotatePitchDownByTime(const float delta_time) {
    rotatePitchDownByDegrees(delta_time * rotation_speed_);
}

void Camera::rotateByMouseOffset(const float xoffset, const float yoffset) {
    yaw_ += xoffset * mouse_sensitivity_;
    pitch_ += yoffset * mouse_sensitivity_;

    clampPitch();
    updateCameraFront();
    LOG_TRACE("Camera rotated by mouse: xoffset=", xoffset, ", yoffset=", yoffset, ", yaw=", yaw_, ", pitch=", pitch_);
}

void Camera::rotateByMouseOffset(const float xoffset, const float yoffset, const float sensitivity) {
    yaw_ += xoffset * sensitivity;
    pitch_ += yoffset * sensitivity;

    clampPitch();
    updateCameraFront();
    LOG_TRACE("Camera rotated by mouse: xoffset=", xoffset, ", yoffset=", yoffset, ", sensitivity=", sensitivity, ", yaw=", yaw_, ", pitch=", pitch_);
}

void Camera::clampPitch() {
    const float previous_pitch = pitch_;
    pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
    if (pitch_ != previous_pitch) {
        LOG_DEBUG("Camera pitch clamped: from=", previous_pitch, ", to=", pitch_);
    }
}

void Camera::updateCameraFront() {
    glm::vec3 front;

    front.x = std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
    front.y = std::sin(glm::radians(pitch_));
    front.z = std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));

    camera_front_ = glm::normalize(front);
    LOG_TRACE("Camera front updated: front=(", camera_front_.x, ", ", camera_front_.y, ", ", camera_front_.z, ")");
}
