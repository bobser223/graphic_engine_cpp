#include <iostream>

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

#include "defines.h"
#include "reader.h"
#include "creator.h"

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

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
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    GLuint vertex_shader = createShaderFromFile(
        GL_VERTEX_SHADER,
        PROJECT_PATH /"tests" /"scene"/"scene_test_001.vert"
    );

    GLuint fragment_shader = createShaderFromFile(
        GL_FRAGMENT_SHADER,
        PROJECT_PATH /"tests" /"scene"/"scene_test_001.frag"
    );

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    GLint model_location = glGetUniformLocation(shader_program, "model");
    GLint view_location = glGetUniformLocation(shader_program, "view");
    GLint projection_location = glGetUniformLocation(shader_program, "projection");

    Model loaded_model = ModelLoader::loadModel(PROJECT_PATH / "data/cube.obj");

    Camera camera;

    glm::mat4 view = camera.getViewMatrix();

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

        glUseProgram(shader_program);

        glm::mat4 model = glm::mat4(1.0f);

        // спочатку центруємо модель у її локальних координатах,
        // потім масштабуємо до нормального розміру
        model = glm::scale(model, glm::vec3(0.15f));
        model = glm::translate(model, glm::vec3(-53.0f, -53.0f, 0.0f));

        glUniformMatrix4fv(
            model_location,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glUniformMatrix4fv(
            view_location,
            1,
            GL_FALSE,
            glm::value_ptr(view)
        );

        glUniformMatrix4fv(
            projection_location,
            1,
            GL_FALSE,
            glm::value_ptr(projection)
        );

        loaded_model.draw(shader_program);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader_program);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}