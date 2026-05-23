#include "Mesh3D.hpp"
#include "OpenGL.hpp"
#include <cstring>
#include <utility>

namespace Champ
{
    Mesh3D::Mesh3D()
    {
        vao = 0;
        std::memset(buffers, 0, MeshBufferType_COUNT * sizeof(uint32_t));
    }

    Mesh3D::Mesh3D(Mesh3D &&other) noexcept
    {
        if(this != &other)
        {
            this->vao = std::exchange(other.vao, 0);
            std::memcpy(buffers, other.buffers, sizeof(buffers));
            std::memset(other.buffers, 0, sizeof(other.buffers));
            vertices = std::move(other.vertices);
            normals = std::move(other.normals);
            uvs = std::move(other.uvs);
            indices = std::move(other.indices);
        }
    }

    Mesh3D &Mesh3D::operator=(Mesh3D &&other) noexcept
    {
        if(this != &other)
        {
            this->vao = std::exchange(other.vao, 0);
            std::memcpy(buffers, other.buffers, sizeof(buffers));
            std::memset(other.buffers, 0, sizeof(other.buffers));
            vertices = std::move(other.vertices);
            normals = std::move(other.normals);
            uvs = std::move(other.uvs);
            indices = std::move(other.indices);
        }
        return *this;
    }

    void Mesh3D::Generate()
    {
        if(vao == 0)
        {
            glGenVertexArrays(1, &vao);
            glGenBuffers(MeshBufferType_COUNT, buffers);
        }

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, buffers[MeshBufferType_Vertex]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, buffers[MeshBufferType_Normal]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), normals.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, buffers[MeshBufferType_UV]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(uvs[0]) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[MeshBufferType_Index]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void Mesh3D::Destroy()
    {
        if(vao)
            glDeleteVertexArrays(1, &vao);

        vao = 0;

        for(uint32_t i = 0; i < MeshBufferType_COUNT; i++)
        {
            if(buffers[i])
                glDeleteBuffers(1, &buffers[i]);
            buffers[i] = 0;
        }
    }

    void Mesh3D::GenerateNormals()
    {
        // 1. Reset all normals to zero first to avoid accumulating onto old data
        for (Vector3 &n : normals)
        {
            n = Vector3(0.0f, 0.0f, 0.0f);
        }

        auto surfaceNormalsFromIndices = [&](uint32_t indexA, uint32_t indexB, uint32_t indexC) -> Vector3 {
            Vector3 pA = vertices[indexA];
            Vector3 pB = vertices[indexB];
            Vector3 pC = vertices[indexC];

            Vector3 sideAB = pB - pA;
            Vector3 sideAC = pC - pA;
            
            return glm::cross(sideAB, sideAC);
        };

        size_t triangleCount = indices.size() / 3;

        for (size_t i = 0; i < triangleCount; i++)
        {
            uint32_t normalTriangleIndex = i * 3;

            uint32_t vertexIndexA = indices[normalTriangleIndex];
            uint32_t vertexIndexB = indices[normalTriangleIndex + 1];
            uint32_t vertexIndexC = indices[normalTriangleIndex + 2];
            
            Vector3 triangleNormal = surfaceNormalsFromIndices(vertexIndexA, vertexIndexB, vertexIndexC);

            normals[vertexIndexA] += triangleNormal;
            normals[vertexIndexB] += triangleNormal;
            normals[vertexIndexC] += triangleNormal;
        }

        for (Vector3 &n : normals)
        {
            if (glm::length(n) > 0.0f)
            {
                n = glm::normalize(n);
            }
        }
    }

    uint32_t Mesh3D::GetVAO() const
    {
        return vao;
    }

    uint32_t Mesh3D::GetVertexCount() const
    {
        return static_cast<uint32_t>(vertices.size());
    }

    uint32_t Mesh3D::GetIndicesCount() const
    {
        return static_cast<uint32_t>(indices.size());
    }

    std::vector<Vector3> &Mesh3D::GetVertices()
    {
        return vertices;
    }

    std::vector<Vector3> &Mesh3D::GetNormals()
    {
        return normals;
    }

    std::vector<Vector2> &Mesh3D::GetUvs()
    {
        return uvs;
    }

    std::vector<uint32_t> &Mesh3D::GetIndices()
    {
        return indices;
    }

    static void SetScale(Mesh3D *mesh, const Vector3 &scale)
    {
        auto &vertices = mesh->GetVertices();
        for(size_t i = 0; i < vertices.size(); i++)
            vertices[i] *= scale;
    }

    Mesh3D MeshGenerator::CreateCube(const Vector3 &scale)
    {
        Mesh3D mesh;

        auto &vertices = mesh.GetVertices();
        auto &uvs = mesh.GetUvs();
        auto &normals = mesh.GetNormals();

        vertices.resize(24);
        uvs.resize(24);
        normals.resize(24);

        vertices[0] = Vector3(0.5f, -0.5f, 0.5f);
        vertices[1] = Vector3(-0.5f, -0.5f, 0.5f);
        vertices[2] = Vector3(0.5f, 0.5f, 0.5f);
        vertices[3] = Vector3(-0.5f, 0.5f, 0.5f);

        vertices[4] = Vector3(0.5f, 0.5f, -0.5f);
        vertices[5] = Vector3(-0.5f, 0.5f, -0.5f);
        vertices[6] = Vector3(0.5f, -0.5f, -0.5f);
        vertices[7] = Vector3(-0.5f, -0.5f, -0.5f);

        vertices[8] = Vector3(0.5f, 0.5f, 0.5f);
        vertices[9] = Vector3(-0.5f, 0.5f, 0.5f);
        vertices[10] = Vector3(0.5f, 0.5f, -0.5f);
        vertices[11] = Vector3(-0.5f, 0.5f, -0.5f);

        vertices[12] = Vector3(0.5f, -0.5f, -0.5f);
        vertices[13] = Vector3(0.5f, -0.5f, 0.5f);
        vertices[14] = Vector3(-0.5f, -0.5f, 0.5f);
        vertices[15] = Vector3(-0.5f, -0.5f, -0.5f);

        vertices[16] = Vector3(-0.5f, -0.5f, 0.5f);
        vertices[17] = Vector3(-0.5f, 0.5f, 0.5f);
        vertices[18] = Vector3(-0.5f, 0.5f, -0.5f);
        vertices[19] = Vector3(-0.5f, -0.5f, -0.5f);

        vertices[20] = Vector3(0.5f, -0.5f, -0.5f);
        vertices[21] = Vector3(0.5f, 0.5f, -0.5f);
        vertices[22] = Vector3(0.5f, 0.5f, 0.5f);
        vertices[23] = Vector3(0.5f, -0.5f, 0.5f);

        uvs[0] = Vector2(0.0f, 0.0f);
        uvs[1] = Vector2(1.0f, 0.0f);
        uvs[2] = Vector2(0.0f, 1.0f);
        uvs[3] = Vector2(1.0f, 1.0f);

        uvs[4] = Vector2(0.0f, 1.0f);
        uvs[5] = Vector2(1.0f, 1.0f);
        uvs[6] = Vector2(0.0f, 1.0f);
        uvs[7] = Vector2(1.0f, 1.0f);
        
        uvs[8] = Vector2(0.0f, 0.0f);
        uvs[9] = Vector2(1.0f, 0.0f);
        uvs[10] = Vector2(0.0f, 0.0f);
        uvs[11] = Vector2(1.0f, 0.0f);
        
        uvs[12] = Vector2(0.0f, 0.0f);
        uvs[13] = Vector2(0.0f, 1.0f);
        uvs[14] = Vector2(1.0f, 1.0f);
        uvs[15] = Vector2(1.0f, 0.0f);
        
        uvs[16] = Vector2(0.0f, 0.0f);
        uvs[17] = Vector2(0.0f, 1.0f);
        uvs[18] = Vector2(1.0f, 1.0f);
        uvs[19] = Vector2(1.0f, 0.0f);
        
        uvs[20] = Vector2(0.0f, 0.0f);
        uvs[21] = Vector2(0.0f, 1.0f);
        uvs[22] = Vector2(1.0f, 1.0f);
        uvs[23] = Vector2(1.0f, 0.0f);

        auto &indices = mesh.GetIndices();
        indices = {
            0, 2, 3,
            0, 3, 1,

            8, 4, 5,
            8, 5, 9,

            10, 6, 7,
            10, 7, 11,

            12, 13, 14,
            12, 14, 15,

            16, 17, 18,
            16, 18, 19,

            20, 21, 22,
            20, 22, 23
        };

        SetScale(&mesh, scale);
        mesh.GenerateNormals();
        mesh.Generate();

        return mesh;
    }
}