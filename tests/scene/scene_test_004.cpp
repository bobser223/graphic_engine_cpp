//
// Created by Volodymyr Avvakumov on 26.05.2026.
//


#include "../../engine/Camera.h"
#include "../../engine/Model.h"
#include "../../engine/Node.h"
#include "../../engine/Shader.h"
#include "../../engine/Texture.h"
#include "../../utils/loader/ModelLoader.h"
#include "Logger.h"
#include "defines.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>


int main() {
    LOG_INFO("Starting scene_test_003");

    if (!glfwInit()) {
        LOG_ERROR("Failed to initialize GLFW");
        return -1;
    }
    LOG_INFO("GLFW initialized");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    constexpr int window_width = 640;
    constexpr int window_height = 480;

    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Model Loader Test", nullptr, nullptr);

    if (window == nullptr) {
        LOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    LOG_INFO("GLFW window created: ", window_width, "x", window_height);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        LOG_ERROR("Failed to initialize GLAD");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    LOG_INFO("GLAD initialized");

    glEnable(GL_DEPTH_TEST);


    Camera camera;


    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);



    constexpr float rotation_speed = 90.0f;
    constexpr float transition_speed = 1.5f;
    float last_frame_time = 0.0f;


    { // Context for RAII shaders, textures and model

        Shader shader(PROJECT_PATH / "tests" / "scene" / "scene_test_004.vert",
                      PROJECT_PATH / "tests" / "scene" / "scene_test_004.frag");

        Texture texture(PROJECT_PATH / "data/texture1.png", TextureType::Diffuse);

        Model loaded_model = ModelLoader::loadModel(PROJECT_PATH / "data/teapot/teapot.obj",
                                                    aiProcess_Triangulate | aiProcess_GenSmoothNormals);

        Node root;
        Node& model_node = root.createChild(&loaded_model);

        model_node.transform_.position_ = glm::vec3(0.0f, 0.42f, -4.0f);
        model_node.transform_.scale_ = glm::vec3(3.0f);

        LOG_INFO("Scene model ready: meshes=", loaded_model.meshes_.size());


        while (!glfwWindowShouldClose(window)) {
            const auto current_frame_time = static_cast<float>(glfwGetTime());
            const float delta_time = current_frame_time - last_frame_time;
            last_frame_time = current_frame_time;


            // ====================== CAMERA ========================

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                camera.goToward(delta_time);
            }

            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                camera.goBack(delta_time);
            }

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                camera.goLeft(delta_time);
            }

            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                camera.goRight(delta_time);
            }

            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                camera.goUp(delta_time);
            }

            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                camera.goDown(delta_time);
            }

            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                camera.rotatePitchUpByDegrees(delta_time * rotation_speed);
            }

            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                camera.rotatePitchDownByDegrees(delta_time * rotation_speed);
            }

            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                camera.rotateYawClockwiseByDegrees(delta_time * rotation_speed);
            }

            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                camera.rotateYawCounterClockwiseByDegrees(delta_time * rotation_speed);
            }

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
            }

            // ====================== ROOT ========================

            if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
                root.transform_.moveDown(delta_time * transition_speed);
            }

            if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
                root.transform_.moveUp(delta_time * transition_speed);
            }

            if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
                root.transform_.moveLeft(delta_time * transition_speed);
            }

            if (glfwGetKey(window, GLFW_KEY_SEMICOLON) == GLFW_PRESS) {
                root.transform_.moveRight(delta_time * transition_speed);
            }



            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.use();

            texture.bind(0);


            shader.setUniform("diffuse_texture", 0);
            shader.setUniform("view", camera.getViewMatrix());
            shader.setUniform("projection", projection);

            root.draw(shader);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    } // end of Context for RAII shaders and textures

    glfwDestroyWindow(window);
    glfwTerminate();

    LOG_INFO("scene_test_003 completed");
    return 0;
}
