cmake_minimum_required(VERSION 3.10)
project(engine)

set(CMAKE_CXX_STANDARD 26)

add_subdirectory(external)
include_directories(utils)

include_directories(utils/loader)
include_directories(utils/points_utils)
include_directories(utils/shader_utils)
include_directories(utils/texture_utils)
include_directories(utils/logger)



find_package(OpenGL REQUIRED)

find_package(assimp REQUIRED)

set(libs
        ${OPENGL_LIBRARY}
        glfw
        glad
        glm
        stb
)

add_executable(main
        scane.cpp
        utils/shader_utils/creator.cpp
        utils/shader_utils/reader.cpp
        utils/texture_utils/texture_creator.cpp
        utils/loader/ModelLoader.cpp
        utils/loader/ModelLoader.h
        engine/Model.cpp
        engine/Model.h
        engine/Mesh.h
        engine/Mesh.cpp
        new_tech_test/fileReaderTest.cpp
        new_tech_test/fileReaderTest.h
        utils/points_utils/points_expander.cpp
        utils/points_utils/points_expander.h
        utils/logger/Logger.cpp
        utils/logger/Logger.h

)

target_link_libraries(main ${libs}
        assimp::assimp
)

add_executable(file_test
        utils/shader_utils/creator.cpp
        utils/shader_utils/reader.cpp
        utils/texture_utils/texture_creator.cpp
        utils/loader/ModelLoader.cpp
        utils/loader/ModelLoader.h
        engine/Model.cpp
        engine/Model.h
        new_tech_test/fileReaderTest.cpp
        utils/defines.h
        utils/points_utils/points_expander.h
        utils/points_utils/points_expander.cpp
        new_tech_test/fileReaderTest.h
        engine/Mesh.cpp
        engine/Material.cpp
        engine/Material.h
        tests/loader/loader_test_001.cpp
        tests/scene/scene_test_001.cpp
        engine/Camera.cpp
        engine/Camera.h
        tests/loader/loader_test_002.cpp
        utils/logger/Logger.cpp
        utils/logger/Logger.h
)

target_link_libraries(file_test ${libs}
        assimp::assimp
)

add_executable(loader_test_002
        tests/loader/loader_test_002.cpp
        utils/shader_utils/creator.cpp
        utils/shader_utils/reader.cpp
        utils/texture_utils/texture_creator.cpp
        utils/loader/ModelLoader.cpp
        utils/loader/ModelLoader.h
        engine/Model.cpp
        engine/Model.h
        engine/Mesh.h
        engine/Mesh.cpp
        new_tech_test/fileReaderTest.cpp
        new_tech_test/fileReaderTest.h
        utils/points_utils/points_expander.cpp
        utils/points_utils/points_expander.h
        utils/logger/Logger.cpp
        utils/logger/Logger.h
)

target_link_libraries(loader_test_002 ${libs}
        assimp::assimp
)


add_executable(scene_test_001
        tests/scene/scene_test_001.cpp
        utils/shader_utils/creator.cpp
        utils/shader_utils/reader.cpp
        utils/texture_utils/texture_creator.cpp
        utils/loader/ModelLoader.cpp
        utils/loader/ModelLoader.h
        engine/Model.cpp
        engine/Model.h
        engine/Mesh.h
        engine/Mesh.cpp
        new_tech_test/fileReaderTest.cpp
        new_tech_test/fileReaderTest.h
        utils/points_utils/points_expander.cpp
        utils/points_utils/points_expander.h
        utils/logger/Logger.cpp
        utils/logger/Logger.h
)

target_link_libraries(scene_test_001 ${libs}
        assimp::assimp
)




