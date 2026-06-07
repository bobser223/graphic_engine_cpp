#ifndef ENGINE_TRANSFORM_H
#define ENGINE_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {
public:
    Transform() = default;

    Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);

    [[nodiscard]] glm::mat4 getMatrix() const;

    void setPosition(const glm::vec3& position);
    void setScale(const glm::vec3& scale);
    void setRotation(const glm::quat& rotation);
    void setRotationEulerDegrees(const glm::vec3& degrees);
    void setRotationAxisAngleDegrees(float degrees, const glm::vec3& axis);
    void rotateAxisAngleDegrees(float degrees, const glm::vec3& axis);

    [[nodiscard]] const glm::vec3& getPosition() const;
    [[nodiscard]] const glm::vec3& getScale() const;
    [[nodiscard]] const glm::quat& getRotation() const;

    void setRotationFromTo(glm::vec3 from, glm::vec3 to);

    void translate(const glm::vec3& delta);
    void translateLocal(const glm::vec3& local_delta);

    [[nodiscard]] glm::vec3 getForward() const;
    [[nodiscard]] glm::vec3 getRight() const;
    [[nodiscard]] glm::vec3 getUp() const;

    void moveForward(float distance);
    void moveBack(float distance);
    void moveRight(float distance);
    void moveLeft(float distance);
    void moveUp(float distance);
    void moveDown(float distance);

    void rotateWorldAxisAngleDegrees(float degrees, const glm::vec3& axis);
    void rotateLocalAxisAngleDegrees(float degrees, const glm::vec3& axis);

    glm::vec3 position_ = glm::vec3(0.0f);
    glm::quat rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale_ = glm::vec3(1.0f);

    [[nodiscard]] static glm::mat4 quaternionToMat4(glm::quat q);
};

#endif // ENGINE_TRANSFORM_H
