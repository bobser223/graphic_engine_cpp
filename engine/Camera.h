//
// Created by Volodymyr Avvakumov on 26.05.2026.
//

#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include <algorithm>
#include <cmath>

#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>

class Camera {
public:
  float camera_speed_ = 3.0f;
  float rotation_speed_ = 90.0f;
  float yaw_ = -90.0f; // -> -z
  float pitch_ = 0.0f;

  float mouse_sensitivity_ = 0.1f;

  glm::vec3 camera_pos_ = glm::vec3(0.0f, 1.5f, 3.0f);    // позиція камери
  glm::vec3 camera_front_ = glm::vec3(0.0f, 0.0f, -1.0f); // куди дивиться
  glm::vec3 camera_up_ = glm::vec3(0.0f, 1.0f, 0.0f);     // up

  [[nodiscard]] inline float getVelocity(const float delta_time) const {
    return delta_time * camera_speed_;
  }

  [[nodiscard]] inline glm::vec3 getCameraRight() const {
    return glm::normalize(glm::cross(camera_front_, camera_up_));
  }

  [[nodiscard]] inline glm::mat4 getViewMatrix() const {
    return glm::lookAt(camera_pos_, camera_pos_ + camera_front_, camera_up_);
  }

  inline void goToward(const float delta_time) {
    camera_pos_ += camera_front_ * getVelocity(delta_time);
  }

  inline void goBack(const float delta_time) {
    camera_pos_ -= camera_front_ * getVelocity(delta_time);
  }

  inline void goRight(const float delta_time) {
    camera_pos_ += getCameraRight() * getVelocity(delta_time);
  }

  inline void goLeft(const float delta_time) {
    camera_pos_ -= getCameraRight() * getVelocity(delta_time);
  }

  inline void goUp(const float delta_time) {
    camera_pos_ += camera_up_ * getVelocity(delta_time);
  }

  inline void goDown(const float delta_time) {
    camera_pos_ -= camera_up_ * getVelocity(delta_time);
  }

  inline void rotateYawClockwiseByDegrees(const float degrees) {
    yaw_ += degrees;
    updateCameraFront();
  }

  inline void rotateYawClockwiseByTime(const float delta_time) {
    rotateYawClockwiseByDegrees(delta_time * rotation_speed_);
  }

  inline void rotateYawCounterClockwiseByDegrees(const float degrees) {
    yaw_ -= degrees;
    updateCameraFront();
  }

  inline void rotateYawCounterClockwiseByTime(const float delta_time) {
    rotateYawCounterClockwiseByDegrees(delta_time * rotation_speed_);
  }

  inline void rotatePitchUpByDegrees(const float degrees) {
    pitch_ += degrees;
    clampPitch();
    updateCameraFront();
  }

  inline void rotatePitchUpByTime(const float delta_time) {
    rotatePitchUpByDegrees(delta_time * rotation_speed_);
  }

  inline void rotatePitchDownByDegrees(const float degrees) {
    pitch_ -= degrees;
    clampPitch();
    updateCameraFront();
  }

  inline void rotatePitchDownByTime(const float delta_time) {
    rotatePitchDownByDegrees(delta_time * rotation_speed_);
  }

  inline void rotateByMouseOffset(const float xoffset, const float yoffset) {
    yaw_ += xoffset * mouse_sensitivity_;
    pitch_ += yoffset * mouse_sensitivity_;

    clampPitch();
    updateCameraFront();
  }

  inline void rotateByMouseOffset(const float xoffset, const float yoffset,
                                  const float sensitivity) {
    yaw_ += xoffset * sensitivity;
    pitch_ += yoffset * sensitivity;

    clampPitch();
    updateCameraFront();
  }

private:
  inline void clampPitch() { pitch_ = std::clamp(pitch_, -89.0f, 89.0f); }

  inline void updateCameraFront() {
    glm::vec3 front;

    front.x = std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
    front.y = std::sin(glm::radians(pitch_));
    front.z = std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));

    camera_front_ = glm::normalize(front);
  }
};

#endif // ENGINE_CAMERA_H
