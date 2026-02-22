#include "Mesh.hpp"

#include <cstring>
#include <filesystem>



struct
{
    float x, y, z;
    float u, v;
} typedef UCMESHVertexInfo;

struct
{
    unsigned int v0, v1, v2;
} typedef UCMESHTriangleInfo;

struct
{
    unsigned int v0, v1, v2, v3;
} typedef UCMESHQuadInfo;



Mesh::Mesh(std::vector<glm::vec3> _vertices, std::vector<unsigned int> _indices, std::vector<glm::vec2> _uvs)
{
    vertices = _vertices;
    indices = _indices;
    uvs = _uvs;

    GenerateBuffers();
}

Mesh::Mesh() {}

Mesh::~Mesh()
{
    DeleteBuffers();
}


void Mesh::ClearVertices()
{
    vertices.clear();
    DeleteBuffers();
}

void Mesh::ClearIndices()
{
    indices.clear();
    DeleteBuffers();
}

void Mesh::ClearUVs()
{
    uvs.clear();
    DeleteBuffers();
}

void Mesh::ClearMesh()
{
    ClearVertices();
    ClearIndices();
    ClearUVs();
}


void Mesh::AddVertexWithUV(glm::vec3 vertex, glm::vec2 uv)
{
    vertices.push_back(vertex);
    uvs.push_back(uv);
}

void Mesh::AddVertexWithUV(float x, float y, float z, float u, float v)
{
    AddVertexWithUV(glm::vec3(x, y, z), glm::vec2(u, v));
}


void Mesh::AddTriangle(unsigned int v0, unsigned int v1, unsigned int v2)
{
    indices.push_back(v0);
    indices.push_back(v1);
    indices.push_back(v2);
}

void Mesh::AddQuad(unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3)
{
    AddTriangle(v3, v0, v1);
    AddTriangle(v1, v2, v3);
}

std::vector<glm::vec3> Mesh::GetVertices() { return vertices; }
std::vector<unsigned int> Mesh::GetIndices() { return indices; }
size_t Mesh::GetIndicesCount() { return indices.size(); }
std::vector<glm::vec2> Mesh::GetUVs() { return uvs; }

bool Mesh::HasBuffers() { return hasbuffers; }

bool Mesh::GenerateBuffers()
{
    if (hasbuffers /*|| vertices.size() == 0 || uvs.size() == 0 || indices.size() == 0*/) return false;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_VERTEX);
    glGenBuffers(1, &VBO_UV);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_VERTEX);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    hasbuffers = true;
    return true;
}

bool Mesh::DeleteBuffers()
{
    if (!hasbuffers) return false;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO_VERTEX);
    glDeleteBuffers(1, &VBO_UV);
    glDeleteBuffers(1, &EBO);

    hasbuffers = false;
    return true;
}

void Mesh::RegenerateBuffers()
{
    GenerateBuffers();
    DeleteBuffers();
}

void Mesh::ApplyTransformation(glm::mat4 mat)
{
    for (size_t i = 0; i < vertices.size(); i++)
    {
        glm::vec4 v = mat * glm::vec4(vertices[i].x, vertices[i].y, vertices[i].z, 0);
        vertices[i] = glm::vec3(v.x, v.y, v.z);
    }
}

void Mesh::ApplyTransformation(Transform t)
{
    ApplyTransformation(t.GetTransformationMatrix());
}

bool Mesh::LoadFromUCMESHFile(std::string filename)
{
    if (!std::filesystem::is_regular_file(filename)) return false;

    FILE *f = fopen(filename.c_str(), "rb");
    if (!f) return false;

    char sig[6];
    fread(&sig, sizeof(char), 6, f);
    if (feof(f) || strncmp(sig, "UCMESH", 6)) goto fileerrorquit;

    uint16_t version;
    fread(&version, sizeof(version), 1, f);
    if (feof(f) || version != 0) goto fileerrorquit;

    uint32_t vertices_count;
    fread(&vertices_count, sizeof(vertices_count), 1, f);
    if (feof(f)) goto fileerrorquit;

    uint32_t primitives_count;
    fread(&primitives_count, sizeof(primitives_count), 1, f);
    if (feof(f)) goto fileerrorquit;

    ClearMesh();

    UCMESHVertexInfo v;
    for (uint32_t i = 0; i < vertices_count; i++)
    {
        fread(&v, sizeof(v), 1, f);
        if (feof(f)) goto readmesherrorquit;

        AddVertexWithUV(v.x, v.y, v.z, v.u, v.v);
    }

    uint8_t prim_type;
    for (uint32_t i = 0; i < primitives_count; i++)
    {
        fread(&prim_type, sizeof(prim_type), 1, f);
        if (feof(f)) goto readmesherrorquit;

        switch (prim_type)
        {
            case 0: // triangle.
                UCMESHTriangleInfo tri;
                fread(&tri, sizeof(tri), 1, f);
                if (feof(f)) goto readmesherrorquit;

                if (tri.v0 >= vertices_count || tri.v1 >= vertices_count || tri.v2 >= vertices_count) continue;

                AddTriangle(tri.v0, tri.v1, tri.v2);
                break;

            case 1: // quad.
                UCMESHQuadInfo quad;
                fread(&quad, sizeof(quad), 1, f);
                if (feof(f)) goto readmesherrorquit;

                if (quad.v0 >= vertices_count || quad.v1 >= vertices_count || quad.v2 >= vertices_count || quad.v3 >= vertices_count) continue;

                AddQuad(quad.v0, quad.v1, quad.v2, quad.v3);
                break;

            default:
                goto readmesherrorquit;
        }
    }

    GenerateBuffers();

    fclose(f);
    return true;

    readmesherrorquit:
        ClearMesh();
    fileerrorquit:
        fclose(f);
    return false;
}

bool Mesh::RenderMesh()
{
    if (!HasBuffers()) return false;

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, GetIndicesCount(), GL_UNSIGNED_INT, nullptr);

    return true;
}