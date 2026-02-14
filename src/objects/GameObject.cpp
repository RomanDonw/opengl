#include "GameObject.hpp"

#include <algorithm>

GameObject::GameObject(Transform t) : transform(t) {}
GameObject::GameObject() {}

GameObject::~GameObject() { SetParent(nullptr); }

GameObject GameObject::Copy() { return *this; }

void GameObject::SetParent(GameObject *new_parent)
{
    if (parent) parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end());
    if (new_parent) new_parent->children.push_back(this);
    parent = new_parent;
}

glm::mat4 GameObject::GetParentGlobalTransformationMatrix()
{ return parent ? parent->GetParentGlobalTransformationMatrix() * parent->transform.GetTransformationMatrix() : glm::mat4(1.0f); }