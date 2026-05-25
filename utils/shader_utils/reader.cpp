//
// Created by Volodymyr Avvakumov on 11.04.2026.
//

#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

std::string readShaderFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open shader file: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}