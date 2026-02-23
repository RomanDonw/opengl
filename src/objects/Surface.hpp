#ifndef SURFACE_HPP
#define SURFACE_HPP

#include "../glm.hpp"

#include "Transform.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"

enum
{
    NoCulling = 0,
    BackFace = 1,
    FrontFace = 2,
    BothFaces = 3
} typedef FaceCullingType;

class Surface
{
    private:
        Texture *texture = nullptr;
        Mesh *mesh = nullptr;

    public:
        Transform transform = Transform();

        glm::vec4 color = glm::vec4(1.0f);
        bool enableRender = true;
        FaceCullingType culling = BackFace;

        Surface(Transform tr, Texture *t, Mesh *m, FaceCullingType c);
        Surface(Texture *t, Mesh *m, FaceCullingType c);

        Surface(Transform tr, Mesh *m, Texture *t, FaceCullingType c);
        Surface(Mesh *m, Texture *t, FaceCullingType c);

        Surface(Transform tr, Texture *t, Mesh *m);
        Surface(Texture *t, Mesh *m);

        Surface(Transform tr, Mesh *m, Texture *t);
        Surface(Mesh *m, Texture *t);

        Surface(Transform tr, Mesh *m);
        Surface(Mesh *m);

        ~Surface();

        //Surface Copy();

        Texture *GetTexture();
        Mesh *GetMesh();

        void SetTexture(Texture *t);
        void SetMesh(Mesh *m);
};

#endif