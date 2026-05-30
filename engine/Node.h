//
// Created by Volodymyr Avvakumov on 27.05.2026.
//

#ifndef ENGINE_NODE_H
#define ENGINE_NODE_H
#include <memory>
#include <vector>

#include "Model.h"
#include "Shader.h"
#include "Transform.h"


class Node {
public:
    Transform transform_;
    Node* parent_= nullptr;
    Model* model_=nullptr;

    std::vector<std::unique_ptr<Node>> children_;

    Node() = default;

    explicit Node(Model* model)
        : model_(model) {}

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    Node(Node&&) noexcept = delete;
    Node& operator=(Node&&) noexcept = delete;
    
    Node& addChild(std::unique_ptr<Node> child) {
        child->parent_ = this;
        children_.push_back(std::move(child));
        return *children_.back();
    }

    Node& createChild() {
        auto child = std::make_unique<Node>();
        child->parent_ = this;

        children_.push_back(std::move(child));
        return *children_.back();
    }

    Node& createChild(Model* model) {
        assert(model != nullptr);
        
        auto child = std::make_unique<Node>(model);
        child->parent_ = this;

        children_.push_back(std::move(child));
        return *children_.back();
    }

    [[nodiscard]] glm::mat4 getLocalMatrix() const {
        return transform_.getMatrix();
    }

    [[nodiscard]] glm::mat4 getWorldMatrix() const {
        const Node* parent = parent_;
        glm::mat4 result = getLocalMatrix();
        while ( parent != nullptr) {
            result = parent->getLocalMatrix() * result;
            parent = parent->parent_;
        }

        return result;
    }

    void draw(const Shader& shader, const glm::mat4& parent_matrix = glm::mat4(1.0f)) const {
        const glm::mat4 world_matrix = parent_matrix * getLocalMatrix();

        if (model_ != nullptr) {
            shader.setUniform("model", world_matrix);
            model_->draw(shader.getProgramId());
        }

        for (const auto& child : children_) {
            child->draw(shader, world_matrix);
        }
    }
};

#endif //ENGINE_NODE_H
