//
// Created by Volodymyr Avvakumov on 22.05.2026.
//

#ifndef ENGINE_DEFINES_H
#define ENGINE_DEFINES_H
#include <filesystem>


typedef unsigned int idx;
typedef unsigned int uint;

// ->  utils -> graphics_engine
inline const std::filesystem::path PROJECT_PATH =std::filesystem::path(__FILE__).parent_path().parent_path();

#endif //ENGINE_DEFINES_H
