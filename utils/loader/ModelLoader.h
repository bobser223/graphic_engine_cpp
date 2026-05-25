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

    static Model loadModel(const std::string& path, unsigned int flags) { //TODO: use general importer
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, flags);

        Model model;
        std::vector<Mesh> mashes((int)scene->mNumMeshes);
        for (int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh* ai_mesh = scene->mMeshes[i];

            auto verts = ai_mesh->mVertices;
            auto indices = ai_mesh->mFaces->mIndices;
            auto normals = ai_mesh->mNormals;


            Mesh mash;



        }
        model.meshes_ = mashes;

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


    std::vector<Vertex> extractVerticesFromAiMesh(const aiMesh* ai_mesh) {
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

};


#endif //ENGINE_MODELLOADER_H
