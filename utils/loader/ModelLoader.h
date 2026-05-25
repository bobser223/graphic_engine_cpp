//
// Created by Volodymyr Avvakumov on 22.05.2026.
//


#ifndef ENGINE_MODELLOADER_H
#define ENGINE_MODELLOADER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../../engine/Model.h"
#include "../../engine/Mesh.h"



class ModelLoader {
private:
    Assimp::Importer importer;
    const aiScene* scene;

public:

    ModelLoader() = default;
    ~ModelLoader() = default;

    static Model loadModel(const std::string& path, unsigned int flags = aiProcess_Triangulate) { //TODO: use general importer
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, flags);

        Model model;
        model.directory_ = path;

        model.meshes_.reserve(scene->mNumMeshes);

        for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
            aiMesh* ai_mesh = scene->mMeshes[i];

            auto vertices = extractVerticesFromAiMesh(ai_mesh);
            auto indices = extractIndicesFromAiMesh(ai_mesh);

            aiMaterial* ai_material = scene->mMaterials[ai_mesh->mMaterialIndex];
            auto material = extractMaterialFromAiMaterial(ai_material);

            model.meshes_.emplace_back(
                std::move(vertices),
                std::move(indices),
                std::move(material)
            );
        }

        return model;
    }

    static std::vector<double> extractPointsVectorFromAiMesh(aiMesh* mesh, size_t mash_size) {
        constexpr int values_per_vertex = 3;
        std::vector<double> result(mash_size*values_per_vertex);
        for (int i = 0; i < mash_size; i++) {
            result[values_per_vertex*i] = mesh->mVertices[i].x; // aiVector3D vertex = mesh->mVertices[j];
            result[values_per_vertex*i+1] = mesh->mVertices[i].y; // aiVector3D vertex = mesh->mVertices[j];
            result[values_per_vertex*i+2] = mesh->mVertices[i].z; // aiVector3D vertex = mesh->mVertices[j];
        }
        return result;
    }

    static std::vector<idx> extractIndicesVectorFromAiMesh(aiMesh* mesh, size_t mash_size) {
        constexpr int values_per_vertex = 3;
        std::vector<idx> result(mash_size*values_per_vertex);
        for (int i = 0; i < mash_size; i++) {
            result[values_per_vertex*i] = mesh->mFaces[i].mIndices[0];
            result[values_per_vertex*i+1] = mesh->mFaces[i].mIndices[1];
            result[values_per_vertex*i+2] = mesh->mFaces[i].mIndices[2];
        }
        return result;
    }


    static std::vector<Vertex> extractVerticesFromAiMesh(const aiMesh* ai_mesh) {
        if (ai_mesh == nullptr) {
            throw std::runtime_error("Invalid input: ai_mesh is null");
        }

        if (ai_mesh->mVertices == nullptr) {
            throw std::runtime_error("Invalid input: mesh has no vertices");
        }

        if (!ai_mesh->HasNormals()) {
            throw std::runtime_error("Invalid input: mesh has no normals");
        }

        if (!ai_mesh->HasTextureCoords(0)) {
            throw std::runtime_error("Invalid input: mesh has no texture coordinates");
        }

        constexpr uint texture_coords_chanel = 0; // TODO: make the model more versatile, add support of several tex channels

        const aiVector3D* ai_vertices = ai_mesh->mVertices;
        const aiVector3D* ai_normals = ai_mesh->mNormals;
        const aiVector3D* ai_texture_coords = ai_mesh->mTextureCoords[texture_coords_chanel];

        const bool has_tex_coords = ai_mesh->HasTextureCoords(texture_coords_chanel);

        const unsigned int vertex_count = ai_mesh->mNumVertices;

        std::vector<Vertex> result;
        result.reserve(vertex_count);

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

        return result;
    }

    static std::vector<idx> extractIndicesFromAiMesh(const aiMesh* ai_mesh) {
        std::vector<idx> indices;
        constexpr int index_per_vertex = 3;
        indices.reserve(ai_mesh->mNumFaces * index_per_vertex);

        for (unsigned int i = 0; i < ai_mesh->mNumFaces; ++i) {
            const aiFace& face = ai_mesh->mFaces[i];

            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j]);
            }
        }

        return indices;
    }

    static Material extractMaterialFromAiMaterial(const aiMaterial* ai_material) {
        Material material;

        aiColor3D color;

        if (ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            material.diffuse_color = glm::vec3(color.r, color.g, color.b);
        }

        if (ai_material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
            material.specular_color = glm::vec3(color.r, color.g, color.b);
        }

        if (ai_material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
            material.ambient_color = glm::vec3(color.r, color.g, color.b);
        }

        float shininess;
        if (ai_material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
            material.shininess = shininess;
        }

        aiString texture_path;

        if (ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path) == AI_SUCCESS) {
            material.diffuse_texture_path = texture_path.C_Str();
        }

        if (ai_material->GetTexture(aiTextureType_SPECULAR, 0, &texture_path) == AI_SUCCESS) {
            material.specular_texture_path = texture_path.C_Str();
        }

        if (ai_material->GetTexture(aiTextureType_NORMALS, 0, &texture_path) == AI_SUCCESS) {
            material.normal_texture_path = texture_path.C_Str();
        }

        return material;
    }

};


#endif //ENGINE_MODELLOADER_H
