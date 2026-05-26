//
// Created by Volodymyr Avvakumov on 26.05.2026.
//


#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../engine/Mesh.h"
#include "../../engine/Material.h"
#include "../../engine/Model.h"
#include "../../utils/loader/ModelLoader.h"
#include "../../engine/Camera.h"
#include "../../engine/Shader.h"

#include "defines.h"
#include "reader.h"
#include "creator.h"
#include "texture_creator.h"
#include "Logger.h"

int main() {
    LOG_INFO("Starting scene_test_001");

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

    GLFWwindow* window = glfwCreateWindow(
        640,
        480,
        "Model Loader Test",
        nullptr,
        nullptr
    );

    if (window == nullptr) {
        LOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    LOG_INFO("GLFW window created: 640x480");

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        LOG_ERROR("Failed to initialize GLAD");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    LOG_INFO("GLAD initialized");

    glEnable(GL_DEPTH_TEST);


    Shader shader(PROJECT_PATH /"tests" /"scene"/"scene_test_001.vert", PROJECT_PATH /"tests" /"scene"/"scene_test_001.frag");



    GLuint diffuse_texture = createTextureFromFile(PROJECT_PATH / "data/texture1.png");
    if (diffuse_texture == 0) {
        LOG_ERROR("Failed to create diffuse texture for scene_test_002");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    LOG_INFO("Diffuse texture ready: id=", diffuse_texture);

    Model loaded_model = ModelLoader::loadModel(
        PROJECT_PATH / "data/plate/plate.obj",
        aiProcess_Triangulate | aiProcess_GenSmoothNormals
    );
    LOG_INFO("Scene model ready: meshes=", loaded_model.meshes_.size());

    Camera camera;


    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        640.0f / 480.0f,
        0.1f,
        100.0f
    );










    float rotation_speed = 90.0f;
    float last_frame_time = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        auto current_frame_time = static_cast<float>(glfwGetTime());
        float delta_time = current_frame_time - last_frame_time;
        last_frame_time = current_frame_time;


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
            camera.rotatePitchUpByDegrees(delta_time*rotation_speed);
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            camera.rotatePitchDownByDegrees(delta_time*rotation_speed);
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            camera.rotateYawClockwiseByDegrees(delta_time*rotation_speed);
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            camera.rotateYawCounterClockwiseByDegrees(delta_time*rotation_speed);
        }

        glm::mat4 view = camera.getViewMatrix();


        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_texture);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
        model = glm::scale(model, glm::vec3(0.03f));
        model = glm::translate(model, glm::vec3(0.0f, 14.0f, 0.0f));

        shader.setUniform("diffuse_texture", 0);
        shader.setUniform("model", model);
        shader.setUniform("view", camera.getViewMatrix());
        shader.setUniform("projection", projection);

        loaded_model.draw(shader.getProgramId());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &diffuse_texture);

    glfwDestroyWindow(window);
    glfwTerminate();

    LOG_INFO("scene_test_001 completed");
    return 0;
}
