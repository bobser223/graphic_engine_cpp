//
// Created by Volodymyr Avvakumov on 22.05.2026.
//


#ifndef ENGINE_MODELLOADER_H
#define ENGINE_MODELLOADER_H

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

#include "../../engine/Model.h"
#include "../../engine/Mesh.h"

class ModelLoader {
private:
    Assimp::Importer importer;
    const aiScene* scene = nullptr;

public:
    ModelLoader() = default;
    ~ModelLoader() = default;

    static Model loadModel(const std::string& path, unsigned int flags = aiProcess_Triangulate);
    static Model loadModel(const std::filesystem::path& path, unsigned int flags = aiProcess_Triangulate);

    static std::vector<double> extractPointsVectorFromAiMesh(aiMesh* mesh, size_t mash_size);
    static std::vector<idx> extractIndicesVectorFromAiMesh(aiMesh* mesh, size_t mash_size);
    static std::vector<Vertex> extractVerticesFromAiMesh(const aiMesh* ai_mesh);
    static std::vector<idx> extractIndicesFromAiMesh(const aiMesh* ai_mesh);
    static Material extractMaterialFromAiMaterial(const aiMaterial* ai_material);

};


#endif //ENGINE_MODELLOADER_H
