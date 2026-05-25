//
// Created by Volodymyr Avvakumov on 25.05.2026.
//

#include "../../engine/Mesh.h"
#include "../../engine/Material.h"
#include "../../engine/Model.h"
#include "../../utils/loader/ModelLoader.h"
#include "defines.h"


int main() {

    auto a = ModelLoader::loadModel(PROJECT_PATH/ "data/cube.obj");
    std::cout << a << std::endl;



}