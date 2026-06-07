//
// Created by Volodymyr Avvakumov on 27.05.2026.
//

#ifndef ENGINE_NODE_H
#define ENGINE_NODE_H
#include "Transform.h"

#include <glm/mat4x4.hpp>
#include <memory>
#include <vector>

class Model;
class Shader;

class Node {
public:
    Transform transform_;
    Node* parent_ = nullptr;
    Model* model_ = nullptr;

    std::vector<std::unique_ptr<Node>> children_;

    Node() = default;

    explicit Node(Model* model);

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    Node(Node&&) noexcept = delete;
    Node& operator=(Node&&) noexcept = delete;

    Node& addChild(std::unique_ptr<Node> child);
    Node& createChild();
    Node& createChild(Model* model);

    [[nodiscard]] glm::mat4 getLocalMatrix() const;
    [[nodiscard]] glm::mat4 getWorldMatrix() const;

    void draw(const Shader& shader, const glm::mat4& parent_matrix = glm::mat4(1.0f)) const;
};

#endif // ENGINE_NODE_H
