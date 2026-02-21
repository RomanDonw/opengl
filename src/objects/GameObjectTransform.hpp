#ifndef GAMEOBJECTTRANSFORM_HPP
#define GAMEOBJECTTRANSFORM_HPP

#include "Transform.hpp"

class GameObject;

class GameObjectTransform : public Transform
{
    friend class GameObject;

    private:
        GameObject *object;

        void OnTransformChanged() override;

        GameObjectTransform(GameObject *obj);

    public:
        GameObjectTransform *operator=(Transform other);
};

#endif