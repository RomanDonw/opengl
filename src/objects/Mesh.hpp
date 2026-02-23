#ifndef MESH_HPP
#define MESH_HPP

#include "../glm.hpp"
#include "../opengl.hpp"

#include <vector>
#include <string>

#include "Transform.hpp"

class Surface;

class Mesh
{
    friend class Surface;

    private:
        std::vector<glm::vec3> vertices;
        std::vector<unsigned int> indices;
        std::vector<glm::vec2> uvs;

        bool hasbuffers = false;
        GLuint VAO, VBO_VERTEX, VBO_UV, EBO;

        std::vector<Surface *> attached_surfaces = std::vector<Surface *>();

    public:
        Mesh(std::vector<glm::vec3> _vertices, std::vector<unsigned int> _indices, std::vector<glm::vec2> _uvs);
        Mesh();

        ~Mesh();

        //Mesh Copy() { return *this; }

        void ClearVertices();
        void ClearIndices();
        void ClearUVs();
        void ClearMesh();

        void AddVertexWithUV(glm::vec3 vertex, glm::vec2 uv);
        void AddVertexWithUV(float x, float y, float z, float u, float v);

        void AddTriangle(unsigned int v0, unsigned int v1, unsigned int v2);

        /*
        Vertices sequence:

            v0 ... v1
            .       .
            .       .
            v3 ... v2

        */
        void AddQuad(unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3);

        std::vector<glm::vec3> GetVertices();
        std::vector<unsigned int> GetIndices();
        size_t GetIndicesCount();
        std::vector<glm::vec2> GetUVs();

        bool HasBuffers();
        bool GenerateBuffers();
        bool DeleteBuffers();
        void RegenerateBuffers();

        void ApplyTransformation(glm::mat4 mat);
        void ApplyTransformation(Transform t);

        bool LoadFromUCMESHFile(std::string filename);
        bool RenderMesh();
};

#endif