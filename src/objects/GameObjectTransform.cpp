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
    position = other.GetPosition();
    rotation = other.GetRotation();
    scale = other.GetScale();

    return this;
}