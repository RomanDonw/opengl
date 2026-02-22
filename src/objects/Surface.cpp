#include "Surface.hpp"

Surface::Surface(Transform tr, Texture *t, Mesh *m, FaceCullingType c)
{
    SetTexture(t);
    SetMesh(m);

    transform = tr;
    culling = c;
}

Surface::Surface(Texture *t, Mesh *m, FaceCullingType c)
{
    SetTexture(t);
    SetMesh(m);

    culling = c;
}

Surface::Surface(Transform tr, Mesh *m, Texture *t, FaceCullingType c)
{
    SetTexture(t);
    SetMesh(m);

    transform = tr;
    culling = c;
}

Surface::Surface(Mesh *m, Texture *t, FaceCullingType c)
{
    SetTexture(t);
    SetMesh(m);

    culling = c;
}

Surface::Surface(Transform tr, Texture *t, Mesh *m)
{
    SetTexture(t);
    SetMesh(m);

    transform = tr;
}

Surface::Surface(Texture *t, Mesh *m)
{
    SetTexture(t);
    SetMesh(m);
}

Surface::Surface(Transform tr, Mesh *m, Texture *t)
{
    SetTexture(t);
    SetMesh(m);

    transform = tr;
}

Surface::Surface(Mesh *m, Texture *t)
{
    SetTexture(t);
    SetMesh(m);
}

Surface::Surface(Transform tr, Mesh *m)
{
    SetMesh(m);

    transform = tr;
}

Surface::Surface(Mesh *m)
{
    SetMesh(m);
}

Surface::~Surface() {}

/*
Surface Copy()
{
    return *this;
}
*/

Texture *Surface::GetTexture() { return texture; }
Mesh *Surface::GetMesh() { return mesh; }

void Surface::SetTexture(Texture *t)
{
    texture = t;
}

void Surface::SetMesh(Mesh *m)
{
    mesh = m;
}