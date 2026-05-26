//
// Created by Volodymyr Avvakumov on 26.05.2026.
//

#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H
#include "glm/vec3.hpp"

class Camera {
public:
  float camera_speed_;
  float yaw_;
  float pitch_;

  glm::vec3 camera_pos_ = glm::vec3(0.0f, 1.5f, 3.0f);    // позиція камери
  glm::vec3 camera_front_ = glm::vec3(0.0f, 0.0f, -1.0f); // куди дивиться
  glm::vec3 camera_up_ = glm::vec3(0.0f, 1.0f, 0.0f);     // up

  inline float get_velocity(float delta_time) {
    return delta_time * camera_speed_;
  }

  inline glm::vec3 get_camera_right() {
    return glm::normalize(glm::cross(camera_front_, camera_up_));
  }

  void go_toward(float delta_time) {

    camera_pos_ += camera_front_ * get_velocity(delta_time);
  }

  void go_back(float delta_time) {

    camera_pos_ -= camera_front_ * get_velocity(delta_time);
  }

  void go_right(float delta_time) {
    camera_pos_ += get_camera_right() * get_velocity(delta_time);
  }

  void go_left(float delta_time) {
    camera_pos_ -= get_camera_right() * get_velocity(delta_time);
  }

  void go_up(float delta_time) {
    camera_pos_ += camera_up_ * get_velocity(delta_time);
  }

  void go_down(float delta_time) {
    camera_pos_ -= camera_up_ * get_velocity(delta_time);
  }
};

#endif // ENGINE_CAMERA_H
