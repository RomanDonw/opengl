#include "GameObjectTransform.hpp"

#include "GameObject.hpp"

#include <iostream>

// === PRIVATE ===

GameObjectTransform::GameObjectTransform(GameObject *obj) : Transform(), object(obj) {}

void GameObjectTransform::OnTransformChanged()
{
    Transform::OnTransformChanged();
    
    object->OnLocalTransformChanged();
}

GameObjectTransform *GameObjectTransform::operator=(Transform other)
{
    Transform::operator=(other);
    return this;
}