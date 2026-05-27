//
// Created by Volodymyr Avvakumov on 26.05.2026.
//

#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include <glm/mat4x4.hpp>
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

  [[nodiscard]] float getVelocity(float delta_time) const;
  [[nodiscard]] glm::vec3 getCameraRight() const;
  [[nodiscard]] glm::mat4 getViewMatrix() const;

  void goToward(float delta_time);
  void goBack(float delta_time);
  void goRight(float delta_time);
  void goLeft(float delta_time);
  void goUp(float delta_time);
  void goDown(float delta_time);

  void rotateYawClockwiseByDegrees(float degrees);
  void rotateYawClockwiseByTime(float delta_time);
  void rotateYawCounterClockwiseByDegrees(float degrees);
  void rotateYawCounterClockwiseByTime(float delta_time);
  void rotatePitchUpByDegrees(float degrees);
  void rotatePitchUpByTime(float delta_time);
  void rotatePitchDownByDegrees(float degrees);
  void rotatePitchDownByTime(float delta_time);
  void rotateByMouseOffset(float xoffset, float yoffset);
  void rotateByMouseOffset(float xoffset, float yoffset, float sensitivity);

private:
  void clampPitch();
  void updateCameraFront();
};

#endif // ENGINE_CAMERA_H
