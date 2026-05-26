//
// Created by Volodymyr Avvakumov on 26.05.2026.
//


//
// Created by Volodymyr Avvakumov on 25.05.2026.
//

#include "../../engine/Mesh.h"
#include "../../engine/Material.h"
#include "../../engine/Model.h"
#include "../../utils/loader/ModelLoader.h"
#include "defines.h"
#include "Logger.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


int main() {
    LOG_INFO("Starting loader_test_002");

    if (!glfwInit()) {
        LOG_ERROR("Failed to initialize GLFW");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(
        1,
        1,
        "loader_test_002_context",
        nullptr,
        nullptr
    );

    if (window == nullptr) {
        LOG_ERROR("Failed to create GLFW window for OpenGL context");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        LOG_ERROR("Failed to initialize GLAD");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    LOG_INFO("Minimal OpenGL context initialized for loader_test_002");

    auto a = ModelLoader::loadModel(PROJECT_PATH/ "data/cube.obj");
    LOG_INFO("Loaded model: ", a);
    std::cout << a << std::endl;

    glfwDestroyWindow(window);
    glfwTerminate();

    LOG_INFO("loader_test_002 completed");


}
