//
// Created by Volodymyr Avvakumov on 22.05.2026.
//

#include "ModelLoader.h"

#include <stdexcept>
#include <utility>

#include "Logger.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

Model ModelLoader::loadModel(const std::string& path, const unsigned int flags) {
    LOG_INFO("Loading model: ", path, ", flags=", flags);

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, flags);

    if (scene == nullptr) {
        LOG_ERROR("Failed to load model: ", path, ". Assimp error: ", importer.GetErrorString());
        throw std::runtime_error("Failed to load model: " + path + ". Assimp error: " + importer.GetErrorString());
    }

    Model model;
    model.directory_ = path;

    model.meshes_.reserve(scene->mNumMeshes);
    LOG_DEBUG("Model loaded by Assimp: meshes=", scene->mNumMeshes, ", materials=", scene->mNumMaterials);

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* ai_mesh = scene->mMeshes[i];
        LOG_DEBUG("Processing mesh #", i, ": vertices=", ai_mesh->mNumVertices, ", faces=", ai_mesh->mNumFaces);

        auto vertices= extractVerticesFromAiMesh(ai_mesh);
        auto indices = extractIndicesFromAiMesh(ai_mesh);

        aiMaterial* ai_material = scene->mMaterials[ai_mesh->mMaterialIndex];
        auto material = extractMaterialFromAiMaterial(ai_material);

        std::shared_ptr<Texture> diffuse_texture = nullptr;

        if (!material.diffuse_texture_path.empty()) {
            const std::filesystem::path model_path(path);
            const std::filesystem::path model_dir = model_path.parent_path();
            const std::filesystem::path texture_path = model_dir / material.diffuse_texture_path;

            diffuse_texture = std::make_shared<Texture>(
                texture_path.string(),
                TextureType::Diffuse
            );
        }

        model.meshes_.emplace_back(
            std::move(vertices),
            std::move(indices),
            std::move(material),
            ai_mesh->HasTextureCoords(0),
            diffuse_texture
        );
    }

    LOG_INFO("Model loaded successfully: ", path, ", meshes=", model.meshes_.size());
    return model;
}

Model ModelLoader::loadModel(const std::filesystem::path& path, const unsigned int flags) {
    LOG_DEBUG("Loading model from filesystem path: ", path);
    return loadModel(path.string(), flags);
}

std::vector<double> ModelLoader::extractPointsVectorFromAiMesh(aiMesh* mesh, const size_t mash_size) {
    LOG_DEBUG("Extracting point vector from aiMesh: size=", mash_size);

    constexpr int values_per_vertex = 3;
    std::vector<double> result(mash_size * values_per_vertex);
    for (int i = 0; i < mash_size; i++) {
        result[values_per_vertex * i] = mesh->mVertices[i].x;
        result[values_per_vertex * i + 1] = mesh->mVertices[i].y;
        result[values_per_vertex * i + 2] = mesh->mVertices[i].z;
    }
    return result;
}

std::vector<idx> ModelLoader::extractIndicesVectorFromAiMesh(aiMesh* mesh, const size_t mash_size) {
    LOG_DEBUG("Extracting index vector from aiMesh: size=", mash_size);

    constexpr int values_per_vertex = 3;
    std::vector<idx> result(mash_size * values_per_vertex);
    for (int i = 0; i < mash_size; i++) {
        result[values_per_vertex * i] = mesh->mFaces[i].mIndices[0];
        result[values_per_vertex * i + 1] = mesh->mFaces[i].mIndices[1];
        result[values_per_vertex * i + 2] = mesh->mFaces[i].mIndices[2];
    }
    return result;
}

std::vector<Vertex> ModelLoader::extractVerticesFromAiMesh(const aiMesh* ai_mesh) {
    if (ai_mesh == nullptr) {
        LOG_ERROR("Invalid input: ai_mesh is null");
        throw std::runtime_error("Invalid input: ai_mesh is null");
    }

    if (ai_mesh->mVertices == nullptr) {
        LOG_ERROR("Invalid input: mesh has no vertices");
        throw std::runtime_error("Invalid input: mesh has no vertices");
    }

    if (!ai_mesh->HasNormals()) {
        LOG_ERROR("Invalid input: mesh has no normals");
        throw std::runtime_error("Invalid input: mesh has no normals");
    }

    if (!ai_mesh->HasTextureCoords(0)) {
        LOG_WARN("Mesh has no texture coordinates");
    }

    constexpr uint texture_coords_chanel = 0;

    const aiVector3D* ai_vertices = ai_mesh->mVertices;
    const aiVector3D* ai_normals = ai_mesh->mNormals;
    const bool has_tex_coords = ai_mesh->HasTextureCoords(texture_coords_chanel);

    const unsigned int vertex_count = ai_mesh->mNumVertices;

    std::vector<Vertex> result;
    result.reserve(vertex_count);
    LOG_DEBUG("Extracting vertices from aiMesh: count=", vertex_count);

    for (unsigned int i = 0; i < vertex_count; ++i) {
        Vertex vertex;

        vertex.position_ = glm::vec3(
            ai_vertices[i].x,
            ai_vertices[i].y,
            ai_vertices[i].z
        );

        vertex.normal_ = glm::vec3(
            ai_normals[i].x,
            ai_normals[i].y,
            ai_normals[i].z
        );

        vertex.tex_coord_ = has_tex_coords
            ? glm::vec2(ai_mesh->mTextureCoords[texture_coords_chanel][i].x, ai_mesh->mTextureCoords[texture_coords_chanel][i].y)
            : glm::vec2(0.0f, 0.0f);

        result.push_back(vertex);
    }

    LOG_INFO("Extracted vertices: count=", result.size());
    return result;
}

std::vector<idx> ModelLoader::extractIndicesFromAiMesh(const aiMesh* ai_mesh) {
    std::vector<idx> indices;
    constexpr int index_per_vertex = 3;
    indices.reserve(ai_mesh->mNumFaces * index_per_vertex);
    LOG_DEBUG("Extracting indices from aiMesh: faces=", ai_mesh->mNumFaces);

    for (unsigned int i = 0; i < ai_mesh->mNumFaces; ++i) {
        const aiFace& face = ai_mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    LOG_INFO("Extracted indices: count=", indices.size());
    return indices;
}

Material ModelLoader::extractMaterialFromAiMaterial(const aiMaterial* ai_material) {
    Material material;
    LOG_DEBUG("Extracting material from aiMaterial");

    aiColor3D color;

    if (ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        material.diffuse_color = glm::vec3(color.r, color.g, color.b);
        LOG_DEBUG("Material diffuse color: ", color.r, ", ", color.g, ", ", color.b);
    }

    if (ai_material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
        material.specular_color = glm::vec3(color.r, color.g, color.b);
        LOG_DEBUG("Material specular color: ", color.r, ", ", color.g, ", ", color.b);
    }

    if (ai_material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
        material.ambient_color = glm::vec3(color.r, color.g, color.b);
        LOG_DEBUG("Material ambient color: ", color.r, ", ", color.g, ", ", color.b);
    }

    float shininess;
    if (ai_material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
        material.shininess = shininess;
        LOG_DEBUG("Material shininess: ", shininess);
    }

    aiString texture_path;

    if (ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path) == AI_SUCCESS) {
        material.diffuse_texture_path = texture_path.C_Str();
        LOG_DEBUG("Material diffuse texture: ", material.diffuse_texture_path);
    }

    if (ai_material->GetTexture(aiTextureType_SPECULAR, 0, &texture_path) == AI_SUCCESS) {
        material.specular_texture_path = texture_path.C_Str();
        LOG_DEBUG("Material specular texture: ", material.specular_texture_path);
    }

    if (ai_material->GetTexture(aiTextureType_NORMALS, 0, &texture_path) == AI_SUCCESS) {
        material.normal_texture_path = texture_path.C_Str();
        LOG_DEBUG("Material normal texture: ", material.normal_texture_path);
    }

    LOG_INFO("Material extracted: ", material);
    return material;
}
