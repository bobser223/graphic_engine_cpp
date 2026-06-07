//
// Created by Volodymyr Avvakumov on 26.05.2026.
//


#include "../../engine/Camera.h"
#include "../../engine/Model.h"
#include "../../engine/Node.h"
#include "../../engine/Shader.h"
#include "../../engine/Texture.h"
#include "../../utils/loader/ModelLoader.h"
#include "../../engine/Light.h"
#include "Logger.h"
#include "defines.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

void processKeyboardInput(
    GLFWwindow* window,
    Camera& camera,
    Node& node_1,
    Node& node_light,
    const float delta_time,
    const float rotation_speed,
    const float transition_speed
) {
    // ====================== MOUSE CAMERA ROTATION ========================

    static bool first_mouse = true;
    static double last_mouse_x = 0.0;
    static double last_mouse_y = 0.0;

    double mouse_x = 0.0;
    double mouse_y = 0.0;

    glfwGetCursorPos(window, &mouse_x, &mouse_y);

    if (first_mouse) {
        last_mouse_x = mouse_x;
        last_mouse_y = mouse_y;
        first_mouse = false;
    }

    const float xoffset = static_cast<float>(mouse_x - last_mouse_x);
    const float yoffset = static_cast<float>(last_mouse_y - mouse_y);

    last_mouse_x = mouse_x;
    last_mouse_y = mouse_y;

    camera.rotateByMouseOffset(xoffset, yoffset);

    // ====================== CAMERA MOVEMENT ========================

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

    // arrows backup
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

    // ====================== MODEL ========================

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        node_1.transform_.moveDown(delta_time * transition_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        node_1.transform_.moveUp(delta_time * transition_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        node_1.transform_.moveLeft(delta_time * transition_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_SEMICOLON) == GLFW_PRESS) {
        node_1.transform_.moveRight(delta_time * transition_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_APOSTROPHE) == GLFW_PRESS) {
        node_1.transform_.moveForward(delta_time * transition_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_BACKSLASH) == GLFW_PRESS) {
        node_1.transform_.moveBack(delta_time * transition_speed);
    }

    // ====================== LIGHT ========================

    // ====================== LIGHT ========================
    // Аналогічно до керування моделлю:
    // U = J           вниз
    // I = K           вгору
    // O = L           вліво
    // P = ;           вправо
    // [ = '           вперед
    // ] = \           назад

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        node_light.transform_.moveDown(delta_time * transition_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        node_light.transform_.moveUp(delta_time * transition_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        node_light.transform_.moveLeft(delta_time * transition_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        node_light.transform_.moveRight(delta_time * transition_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
        node_light.transform_.moveForward(delta_time * transition_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
        node_light.transform_.moveBack(delta_time * transition_speed);
    }
}


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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Capture mouse cursor for fly camera rotation

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

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);


    Camera camera;


    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);



    constexpr float rotation_speed = 90.0f;
    constexpr float transition_speed = 1.5f;
    float last_frame_time = 0.0f;


    { // Context for RAII shaders, textures and model

        Shader shader(PROJECT_PATH / "tests" / "scene" / "scene_test_008.vert",
                      PROJECT_PATH / "tests" / "scene" / "scene_test_008.frag");

        auto texture = std::make_shared<Texture>((PROJECT_PATH / "data/texture1.png").string(), TextureType::Diffuse);
        auto white_texture = std::make_shared<Texture>((PROJECT_PATH / "data/textures/white.jpg").string(), TextureType::Diffuse);


        Model loaded_model = ModelLoader::loadModel(PROJECT_PATH / "data/mmf/faculty.obj",
                                                    aiProcess_Triangulate | aiProcess_GenSmoothNormals);

        Model cube_for_light = ModelLoader::loadModel(PROJECT_PATH / "data/cube.obj",aiProcess_Triangulate | aiProcess_GenSmoothNormals);

        // for (Mesh& mesh : loaded_model.meshes_) {
        //     mesh.setDiffuseTexture(texture);
        // }

        // for (Mesh& mesh : loaded_model.meshes_) {
        //     mesh.setMaterial(Material::Gold());
        // }

        // for (Mesh& mesh : loaded_model.meshes_) {
        //     mesh.setMaterial(Material::Bronze());
        // }
        for (Mesh& mesh : loaded_model.meshes_) {
            mesh.setMaterial(Material::Plastic());
        }

        // ========== Light cube ======

        for (Mesh& mesh : cube_for_light.meshes_) {
            mesh.setDiffuseTexture(white_texture);
        }

        // for (Mesh& mesh : loaded_model.meshes_) {
        //     mesh.disableTexture();
        // }

        Node root;
        Node& model_node = root.createChild(&loaded_model);

        Node& light_node = root.createChild(&cube_for_light);


        model_node.transform_.position_ = glm::vec3(0.0f, 0.42f, -4.0f);
        model_node.transform_.scale_ = glm::vec3(0.030f);

        // light_node.transform_.position_ = glm::vec3(2.0f, 4.0f, 2.0f);
        light_node.transform_.position_ = model_node.transform_.position_ + glm::vec3(0.0f, 1.5f, 0.0f);
        light_node.transform_.scale_ = glm::vec3(0.009f);


        LOG_INFO("Scene model ready: meshes=", loaded_model.meshes_.size());


        Light light(light_node.transform_.position_, glm::vec3(1.0f), 1.0f);


        while (!glfwWindowShouldClose(window)) {
            const auto current_frame_time = static_cast<float>(glfwGetTime());
            const float delta_time = current_frame_time - last_frame_time;
            last_frame_time = current_frame_time;


            processKeyboardInput(
                window,
                camera,
                model_node,
                light_node,
                delta_time,
                rotation_speed,
                transition_speed
            );

            light.position = light_node.transform_.position_;


            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.use();

            shader.setUniform("global_ambient_strength", 0.15f);
            shader.setUniform("diffuse_texture", 0);
            shader.setUniform("view", camera.getViewMatrix());
            shader.setUniform("projection", projection);
            shader.setUniform("light.position", light.position);
            shader.setUniform("light.color", light.color);
            shader.setUniform("light.intensity", light.intensity);

            shader.setUniform("view_pos", camera.camera_pos_);

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
//
// Created by Volodymyr Avvakumov on 07.06.2026.
//
