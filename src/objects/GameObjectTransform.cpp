#include "GameObjectTransform.hpp"

#include "GameObject.hpp"

#include <iostream>

// === PRIVATE ===

GameObjectTransform::GameObjectTransform(GameObject *obj) : object(obj) {}

void GameObjectTransform::OnTransformChanged()
{
    Transform::OnTransformChanged();
    
    object->OnLocalTransformChanged();
}

GameObjectTransform *GameObjectTransform::operator=(Transform other)
{
    position = other.position;
    rotation = other.rotation;
    scale = other.scale;

    UpdateCache();

    return this;
}