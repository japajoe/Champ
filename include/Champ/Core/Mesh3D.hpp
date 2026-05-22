#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include <vector>

namespace Champ
{
    enum MeshBufferType
    {
        MeshBufferType_Vertex,
        MeshBufferType_Normal,
        MeshBufferType_UV,
        MeshBufferType_Index,
        MeshBufferType_COUNT
    };

    class Mesh3D
    {
    public:
        Mesh3D();
        Mesh3D(const Mesh3D &other) = delete;
        Mesh3D(Mesh3D &&other) noexcept;
        Mesh3D &operator=(const Mesh3D &other) = delete;
        Mesh3D &operator=(Mesh3D &&other) noexcept;
        void Generate();
        void Destroy();
        void GenerateNormals();
        uint32_t GetVAO() const;
        uint32_t GetVertexCount() const;
        uint32_t GetIndicesCount() const;
        std::vector<Vector3> &GetVertices();
        std::vector<Vector3> &GetNormals();
        std::vector<Vector2> &GetUvs();
        std::vector<uint32_t> &GetIndices();
    private:
        uint32_t vao;
        uint32_t buffers[MeshBufferType_COUNT];
        std::vector<Vector3> vertices;
        std::vector<Vector3> normals;
        std::vector<Vector2> uvs;
        std::vector<uint32_t> indices;
    };

    class MeshGenerator
    {
    public:
        static Mesh3D CreateCube();
    };
}