#include "Surface.hpp"

#include <vector>
#include <iterator>
#include <algorithm>

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

Surface::~Surface()
{
    SetTexture(nullptr);
    SetMesh(nullptr);
}

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
    //texture = t;
    if (texture) texture->attached_surfaces.erase(std::remove(texture->attached_surfaces.begin(), texture->attached_surfaces.end(), this), texture->attached_surfaces.end());
    if (t) t->attached_surfaces.push_back(this);

    texture = t;
}

void Surface::SetMesh(Mesh *m)
{
    //mesh = m;
    if (mesh) mesh->attached_surfaces.erase(std::remove(mesh->attached_surfaces.begin(), mesh->attached_surfaces.end(), this), mesh->attached_surfaces.end());
    if (m) m->attached_surfaces.push_back(this);

    mesh = m;
}