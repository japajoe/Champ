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

    static Vector3 PointOnSpheroid(float radius, float height, float horizontalAngle, float verticalAngle)
    {
        float horizontalRadians = glm::radians(horizontalAngle);
        float verticalRadians = glm::radians(verticalAngle);
        float cosVertical = glm::cos(verticalRadians);

        return Vector3(
            radius * glm::sin(horizontalRadians) * cosVertical,
            height * glm::sin(verticalRadians),
            radius * glm::cos(horizontalRadians) * cosVertical);
    }

    static Vector3 PointOnSphere(float radius, float horizontalAngle, float verticalAngle)
    {
        return PointOnSpheroid(radius, radius, horizontalAngle, verticalAngle);
    }

    Mesh3D MeshGenerator::CreateCapsule(const Vector3 &scale)
    {
        Mesh3D mesh;

        auto &vertices = mesh.GetVertices();
        auto &uvs = mesh.GetUvs();
        auto &normals = mesh.GetNormals();
        auto &indices = mesh.GetIndices();

        const float height = 2.0f;
        const float radius = 0.5f;   
        const uint32_t segments = 32;
        const uint32_t rings = 8;
        const float cylinderHeight = height - radius * 2;
        const uint32_t vertexCount = 2 * rings * segments + 2;
        const uint32_t triangleCount = 4 * rings * segments;
        const float horizontalAngle = 360.0f / segments;
        const float verticalAngle = 90.0f / rings;

        vertices.resize(vertexCount);
		normals.resize(vertexCount);
		uvs.resize(vertexCount);
        indices.resize(3 * triangleCount);

        uint32_t vi = 2;
        uint32_t ti = 0;
        uint32_t topCapIndex = 0;
        uint32_t bottomCapIndex = 1;

        vertices[topCapIndex] = Vector3(0, cylinderHeight / 2 + radius, 0);
        normals[topCapIndex] = Vector3(0, 1, 0);
        vertices[bottomCapIndex] = Vector3(0, -cylinderHeight / 2 - radius, 0);
        normals[bottomCapIndex] = Vector3(0, -1, 0);

        for (uint32_t s = 0; s < segments; s++)
        {
            for (uint32_t r = 1; r <= rings; r++)
            {
                // Top cap vertex
                Vector3 normal = PointOnSphere(1, s*horizontalAngle, 90 - r * verticalAngle);
                Vector3 vertex = Vector3(radius*normal.x, radius * normal.y + cylinderHeight / 2, radius * normal.z);
                vertices[vi] = vertex;
                normals[vi] = normal;
                vi++;

                // Bottom cap vertex
                vertices[vi] = Vector3(vertex.x, -vertex.y, vertex.z);
                normals[vi] = Vector3(normal.x, -normal.y, normal.z);
                vi++;

                uint32_t top_s1r1 = vi - 2;
                uint32_t top_s1r0 = vi - 4;
                uint32_t bot_s1r1 = vi - 1;
                uint32_t bot_s1r0 = vi - 3;
                uint32_t top_s0r1 = top_s1r1 - 2 * rings;
                uint32_t top_s0r0 = top_s1r0 - 2 * rings;
                uint32_t bot_s0r1 = bot_s1r1 - 2 * rings;
                uint32_t bot_s0r0 = bot_s1r0 - 2 * rings;

                if (s == 0)
                {
                    top_s0r1 += vertexCount - 2;
                    top_s0r0 += vertexCount - 2;
                    bot_s0r1 += vertexCount - 2;
                    bot_s0r0 += vertexCount - 2;
                }

                // Create cap triangles
                if (r == 1)
                {
                    indices[3 * ti + 0] = topCapIndex;
                    indices[3 * ti + 1] = top_s0r1;
                    indices[3 * ti + 2] = top_s1r1;
                    ti++;

                    indices[3 * ti + 0] = bottomCapIndex;
                    indices[3 * ti + 1] = bot_s1r1;
                    indices[3 * ti + 2] = bot_s0r1;
                    ti++;
                }
                else
                {
                    indices[3 * ti + 0] = top_s1r0;
                    indices[3 * ti + 1] = top_s0r0;
                    indices[3 * ti + 2] = top_s1r1;
                    ti++;

                    indices[3 * ti + 0] = top_s0r0;
                    indices[3 * ti + 1] = top_s0r1;
                    indices[3 * ti + 2] = top_s1r1;
                    ti++;

                    indices[3 * ti + 0] = bot_s0r1;
                    indices[3 * ti + 1] = bot_s0r0;
                    indices[3 * ti + 2] = bot_s1r1;
                    ti++;

                    indices[3 * ti + 0] = bot_s0r0;
                    indices[3 * ti + 1] = bot_s1r0;
                    indices[3 * ti + 2] = bot_s1r1;
                    ti++;
                }
            }

            // Create side triangles
            uint32_t top_s1 = vi - 2;
            uint32_t top_s0 = top_s1 - 2 * rings;
            uint32_t bot_s1 = vi - 1;
            uint32_t bot_s0 = bot_s1 - 2 * rings;
            
            if (s == 0)
            {
                top_s0 += vertexCount - 2;
                bot_s0 += vertexCount - 2;
            }

            indices[3 * ti + 0] = top_s0;
            indices[3 * ti + 1] = bot_s1;
            indices[3 * ti + 2] = top_s1;
            ti++;

            indices[3 * ti + 0] = bot_s0;
            indices[3 * ti + 1] = bot_s1;
            indices[3 * ti + 2] = top_s0;
            ti++;
        }

        SetScale(&mesh, scale);
        mesh.Generate();
        return mesh;
    }

    Mesh3D MeshGenerator::CreateCube(const Vector3 &scale)
    {
        Mesh3D mesh;

        auto &vertices = mesh.GetVertices();
        auto &uvs = mesh.GetUvs();
        auto &normals = mesh.GetNormals();
        auto &indices = mesh.GetIndices();

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

    Mesh3D MeshGenerator::CreateCylinder(const Vector3 &scale)
    {
        Mesh3D mesh;

        auto &vertices = mesh.GetVertices();
        auto &uvs = mesh.GetUvs();
        auto &normals = mesh.GetNormals();
        auto &indices = mesh.GetIndices();

		const float height = 1.0f;
		const float radius = 0.5f;
		const uint32_t slices = 12;
		const float halfHeight = height / 2.0f;

		// Generate the side vertices.
		for (uint32_t slice = 0; slice <= slices; slice++) 
		{
			const float angle = slice * glm::tau<float>() / slices;
			const float x = glm::cos(angle) * (radius / 2.0f);
			const float z = glm::sin(angle) * (radius / 2.0f);
			const float u = (float) slice / slices;

			// Bottom vertex.
			vertices.push_back(Vector3(x, -halfHeight, z));
			normals.push_back(glm::normalize(Vector3(x, 0.0f, z)));
			uvs.push_back(Vector2(u, 1.0f));

			// Top vertex.
			vertices.push_back(Vector3(x, halfHeight, z));
			normals.push_back(glm::normalize(Vector3(x, 0.0f, z)));
			uvs.push_back(Vector2(u, 0.0F));
		}

		// Generate the side indices with flipped winding order.
		for (uint32_t slice = 0; slice < slices; slice++) 
		{
			uint32_t baseIndex = slice * 2;
			uint32_t nextIndex = (slice + 1) * 2;

			// Flip the winding order: swap the order of the indices
			indices.push_back(baseIndex);
			indices.push_back(baseIndex + 1);
			indices.push_back(nextIndex);

			indices.push_back(nextIndex);
			indices.push_back(baseIndex + 1);
			indices.push_back(nextIndex + 1);
		}

		// Generate the bottom cap with flipped winding order.
		const size_t bottomCenterIndex = vertices.size();

		vertices.push_back(Vector3(0, -halfHeight, 0));
		normals.push_back(Vector3(0, -1, 0));
		uvs.push_back(Vector2(0.5f, 0.5f));

		for (uint32_t slice = 0; slice < slices; slice++) 
		{
			uint32_t baseIndex = slice * 2;

			// Flip the winding order
			indices.push_back(bottomCenterIndex);
			indices.push_back(baseIndex);
			indices.push_back(baseIndex + 2);
		}

		// Generate the top cap with flipped winding order.
		const size_t topCenterIndex = vertices.size();

		vertices.push_back(Vector3(0, halfHeight, 0));
		normals.push_back(Vector3(0, 1, 0));
		uvs.push_back(Vector2(0.5f, 0.5f));

		for (uint32_t slice = 0; slice < slices; slice++) 
		{
			uint32_t baseIndex = slice * 2 + 1;

			// Flip the winding order
			indices.push_back(topCenterIndex);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex);
		}

        SetScale(&mesh, scale);
        mesh.Generate();
        return mesh;
    }

    Mesh3D MeshGenerator::CreatePlane(const Vector3 &scale)
    {
        Mesh3D mesh;
        auto &vertices = mesh.GetVertices();
        auto &uvs = mesh.GetUvs();
        auto &normals = mesh.GetNormals();
        auto &indices = mesh.GetIndices();

        vertices.resize(4);
		uvs.resize(4);
		normals.resize(4);

        vertices[0] = Vector3(-0.5f, 0.0f, -0.5f);
        vertices[1] = Vector3(-0.5f, 0.0f,  0.5f);
        vertices[2] = Vector3( 0.5f, 0.0f, -0.5f);
        vertices[3] = Vector3( 0.5f, 0.0f,  0.5f);

        uvs[0] = Vector2(0.0f, 0.0f);
        uvs[1] = Vector2(0.0f, 1.0f);
        uvs[2] = Vector2(1.0f, 0.0f);
        uvs[3] = Vector2(1.0f, 1.0f);

		indices = {
            0, 1, 2,
            2, 1, 3,
        };

        SetScale(&mesh, scale);
        mesh.GenerateNormals();
        mesh.Generate();
        return mesh;
    }

    Mesh3D MeshGenerator::CreateSphere(const Vector3 &scale)
    {
        Mesh3D mesh;

        auto &vertices = mesh.GetVertices();
        auto &uvs = mesh.GetUvs();
        auto &normals = mesh.GetNormals();
        auto &indices = mesh.GetIndices();

        const uint32_t sectorCount = 72;
        const uint32_t stackCount = 24;
		const uint32_t vertexCount = (sectorCount + 1) * (stackCount + 1);
        const float radius = 0.5f;
		const float PI = glm::pi<float>();
        float x, y, z, xy;					// vertex position
        const float lengthInv = 1.0f / radius;    // vertex normal
        float s, t;                         // vertex texCoord
        const float sectorStep = 2 * PI / sectorCount;
        const float stackStep = PI / stackCount;
        float sectorAngle, stackAngle;
		uint32_t vertexIndex = 0;

		vertices.resize(vertexCount);
		uvs.resize(vertexCount);
		normals.resize(vertexCount);

        for(uint32_t i = 0; i <= stackCount; ++i)
        {
            stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
            xy = radius* glm::cos(stackAngle);             // r * cos(u)
            z = radius* glm::sin(stackAngle);              // r * sin(u)

            // add (sectorCount+1) vertices per stack
            // the first and last vertices have same position and normal, but different tex coords
            for(uint32_t j = 0; j <= sectorCount; ++j)
            {
                Vector3 vPosition;
				Vector3 vNormal;
				Vector2 vUV;

                sectorAngle = j * sectorStep;           // starting from 0 to 2pi

                // vertex position (x, y, z)
                x = xy * glm::cos(sectorAngle);             // r * cos(u) * cos(v)
                y = xy * glm::sin(sectorAngle);             // r * cos(u) * sin(v)          
                vPosition = Vector3(x, y, z);

                vNormal.x = x * lengthInv;
                vNormal.y = y * lengthInv;
                vNormal.z = z * lengthInv;

                // vertex tex coord (s, t) range between [0, 1]
                s = (float) j / sectorCount;
                t = (float) i / stackCount;          
                vUV = Vector2(s, t);
                
                vertices[vertexIndex] = vPosition;
				normals[vertexIndex] = vNormal;
				uvs[vertexIndex] = vUV;
				vertexIndex++;
            }
        }

        uint32_t k1, k2;

        for(uint32_t i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // beginning of current stack
            k2 = k1 + sectorCount + 1;      // beginning of next stack

            for(uint32_t j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding first and last stacks
                // k1 => k2 => k1+1
                if(i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                // k1+1 => k2 => k2+1
                if(i != (stackCount-1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }

        SetScale(&mesh, scale);
        mesh.Generate();
        return mesh;
    }
}