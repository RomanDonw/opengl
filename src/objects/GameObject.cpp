#include "GameObject.hpp"

#include <algorithm>
#include <iterator>

#include <iostream>

// === PRIVATE ===

void GameObject::OnLocalTransformChanged() { OnGlobalTransformChanged(); }
void GameObject::OnParentTransformChanged() { OnGlobalTransformChanged(); }

void GameObject::OnGlobalTransformChanged() { for (GameObject *obj : children) obj->OnParentTransformChanged(); }

// === PUBLIC ===

GameObject::GameObject(Transform t) : transform(this) { transform = t; }
GameObject::GameObject() : transform(this) {}

GameObject::~GameObject() { SetParent(nullptr, false); }

GameObject GameObject::Copy() { return *this; }

size_t GameObject::SetParent(GameObject *new_parent, bool save_global_pos)
{
    size_t index = -1; // yea, i know that size_t is an unsigned type, but vector can't contain 2^64 - 1 elements, so i can use that magic number as "special return code/value".
    
    if (parent) parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end());
    if (new_parent)
    {
        new_parent->children.push_back(this);
        index = new_parent->children.size() - 1;
    }

    if (save_global_pos)
    {
        Transform globt = GetGlobalTransform();
        transform = globt - new_parent->GetGlobalTransform();
    }

    parent = new_parent;
    return index;
}

/*glm::mat4 GameObject::GetParentGlobalTransformationMatrix()
{ return parent ? parent->GetParentGlobalTransformationMatrix() * parent->transform.GetTransformationMatrix() : glm::mat4(1.0f); }*/

Transform GameObject::GetParentGlobalTransform()
{ return parent ? parent->GetParentGlobalTransform() + (Transform)parent->transform : Transform(); }

Transform GameObject::GetGlobalTransform()
{ return GetParentGlobalTransform() + (Transform)transform; }