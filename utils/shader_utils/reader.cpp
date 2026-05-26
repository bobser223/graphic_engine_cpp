//
// Created by Volodymyr Avvakumov on 11.04.2026.
//

#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "Logger.h"

std::string readShaderFromFile(const std::string& path) {
    LOG_DEBUG("Reading shader file: ", path);

    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_ERROR("Cannot open shader file: ", path);
        throw std::runtime_error("Cannot open shader file: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    LOG_INFO("Shader file read successfully: ", path, " (", buffer.str().size(), " bytes)");
    return buffer.str();
}
