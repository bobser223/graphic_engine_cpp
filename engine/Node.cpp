//
// Created by Volodymyr Avvakumov on 27.05.2026.
//

#include "Node.h"

#include "Logger.h"
#include "Model.h"
#include "Shader.h"

#include <stdexcept>
#include <utility>

Node::Node(Model* model)
    : model_(model) {
    if (model_ == nullptr) {
        LOG_WARN("Node created with null model pointer");
    } else {
        LOG_DEBUG("Node created with model pointer: ", model_);
    }
}

Node& Node::addChild(std::unique_ptr<Node> child) {
    if (child == nullptr) {
        LOG_ERROR("Cannot add null child node");
        throw std::invalid_argument("Cannot add null child node");
    }

    child->parent_ = this;
    children_.push_back(std::move(child));

    LOG_DEBUG("Child node added: parent=", this, ", child=", children_.back().get(), ", children=", children_.size());
    return *children_.back();
}

Node& Node::createChild() {
    auto child = std::make_unique<Node>();
    return addChild(std::move(child));
}

Node& Node::createChild(Model* model) {
    if (model == nullptr) {
        LOG_ERROR("Cannot create child node with null model pointer");
        throw std::invalid_argument("Cannot create child node with null model pointer");
    }

    auto child = std::make_unique<Node>(model);
    return addChild(std::move(child));
}

glm::mat4 Node::getLocalMatrix() const {
    return transform_.getMatrix();
}

glm::mat4 Node::getWorldMatrix() const {
    const Node* parent = parent_;
    glm::mat4 result = getLocalMatrix();

    while (parent != nullptr) {
        result = parent->getLocalMatrix() * result;
        parent = parent->parent_;
    }

    return result;
}

void Node::draw(const Shader& shader, const glm::mat4& parent_matrix) const {
    const glm::mat4 world_matrix = parent_matrix * getLocalMatrix();

    if (model_ != nullptr) {
        shader.setUniform("model", world_matrix);

        const glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(world_matrix)));

        shader.setUniform("normal_matrix", normal_matrix);

        model_->draw(shader.getProgramId());
    }

    for (const auto& child : children_) {
        child->draw(shader, world_matrix);
    }
}