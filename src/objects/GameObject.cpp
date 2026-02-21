#include "GameObject.hpp"

#include <algorithm>
#include <iterator>

#include <iostream>

// === PRIVATE ===

void GameObject::OnLocalTransformChanged() { OnGlobalTransformChanged(); }
void GameObject::OnParentTransformChanged() { OnGlobalTransformChanged(); }

void GameObject::OnGlobalTransformChanged() { for (GameObject *obj : children) obj->OnParentTransformChanged(); }

// === PUBLIC ===

GameObject::GameObject(Transform t) : transform(this) { transform = t; /*OnLocalTransformChanged();*/ }
GameObject::GameObject() : transform(this) { /*OnLocalTransformChanged();*/ }

GameObject::~GameObject()
{
    SetParent(nullptr, false);
    for (GameObject *obj : children) obj->SetParent(nullptr);
}

//GameObject GameObject::Copy() { return *this; }

size_t GameObject::SetParent(GameObject *new_parent, bool save_global_pos)
{
    size_t index = -1; // yea, i know that size_t is an unsigned type, but vector can't contain 2^64 - 1 elements, so i can use that magic number as "special return code/value".
    if (new_parent == this) return -1;
    
    if (parent) parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end());
    if (new_parent)
    {
        new_parent->children.push_back(this);
        index = new_parent->children.size() - 1;
    }

    OnParentTransformChanged();

    if (save_global_pos)
    {
        Transform globt = GetGlobalTransform();
        transform = globt.GlobalToLocal(new_parent->GetGlobalTransform()); //globt - new_parent->GetGlobalTransform();
    }

    parent = new_parent;
    return index;
}

Transform GameObject::GetParentGlobalTransform()
{ return parent ? (Transform)parent->transform.LocalToGlobal(parent->GetParentGlobalTransform()) : Transform(); }

Transform GameObject::GetGlobalTransform()
{ return (Transform)transform.LocalToGlobal(GetParentGlobalTransform()); }