//
// Created by Volodymyr Avvakumov on 25.05.2026.
//

#include "../../engine/Mesh.h"
#include "../../engine/Material.h"
#include "../../engine/Model.h"
#include "../../utils/loader/ModelLoader.h"
#include "defines.h"
#include "Logger.h"


int main() {
    LOG_INFO("Starting loader_test_001");

    auto a = ModelLoader::loadModel(PROJECT_PATH/ "data/cube.obj");
    LOG_INFO("Loaded model: ", a);
    std::cout << a << std::endl;

    LOG_INFO("loader_test_001 completed");


}
