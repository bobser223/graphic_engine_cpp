#include "../../engine/Camera.h"
#include "../../engine/Material.h"
#include "../../engine/Mesh.h"
#include "../../engine/Model.h"
#include "../../utils/loader/ModelLoader.h"
#include "Logger.h"
#include "creator.h"
#include "defines.h"
#include "reader.h"
#include "texture_creator.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

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

    GLFWwindow* window = glfwCreateWindow(640, 480, "Model Loader Test", nullptr, nullptr);

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

    GLuint vertex_shader =
        createShaderFromFile(GL_VERTEX_SHADER, PROJECT_PATH / "tests" / "scene" / "scene_test_001.vert");

    GLuint fragment_shader =
        createShaderFromFile(GL_FRAGMENT_SHADER, PROJECT_PATH / "tests" / "scene" / "scene_test_001.frag");

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    GLint link_success = 0;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &link_success);
    if (link_success == GL_FALSE) {
        GLint log_length = 0;
        glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &log_length);
        std::string info_log(static_cast<size_t>(log_length), '\0');
        glGetProgramInfoLog(shader_program, log_length, nullptr, info_log.data());
        LOG_ERROR("Shader program link failed: ", info_log);
    } else {
        LOG_INFO("Shader program linked successfully: id=", shader_program);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    GLint model_location = glGetUniformLocation(shader_program, "model");
    GLint view_location = glGetUniformLocation(shader_program, "view");
    GLint projection_location = glGetUniformLocation(shader_program, "projection");
    GLint diffuse_texture_location = glGetUniformLocation(shader_program, "diffuse_texture");

    GLuint diffuse_texture = createTextureFromFile(PROJECT_PATH / "data/texture1.png");
    if (diffuse_texture == 0) {
        LOG_ERROR("Failed to create diffuse texture for scene_test_001");
        glDeleteProgram(shader_program);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    LOG_INFO("Diffuse texture ready: id=", diffuse_texture);

    Model loaded_model = ModelLoader::loadModel(PROJECT_PATH / "data/plate/plate.obj",
                                                aiProcess_Triangulate | aiProcess_GenSmoothNormals);
    LOG_INFO("Scene model ready: meshes=", loaded_model.meshes_.size());

    Camera camera;

    glm::mat4 view = camera.getViewMatrix();

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);


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

        glm::mat4 view = camera.getViewMatrix();


        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_texture);
        glUniform1i(diffuse_texture_location, 0);

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
        model = glm::scale(model, glm::vec3(0.03f));
        model = glm::translate(model, glm::vec3(0.0f, 14.0f, 0.0f));

        glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));

        glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));

        glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));

        loaded_model.draw(shader_program);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader_program);
    glDeleteTextures(1, &diffuse_texture);
    LOG_INFO("Deleted shader program: id=", shader_program);

    glfwDestroyWindow(window);
    glfwTerminate();

    LOG_INFO("scene_test_001 completed");
    return 0;
}
