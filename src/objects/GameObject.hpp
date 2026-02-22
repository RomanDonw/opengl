#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include "../glm.hpp"

#include <vector>

#include "Transform.hpp"
#include "GameObjectTransform.hpp"

enum
{
    UNKNOWN = 0,
    ENTITY = 1,
    CAMERA = 2,
    AUDIOSOURCE = 3,
    AUDIOLISTENER = 4
} typedef GameObjectType;

class GameObject
{
    friend class GameObjectTransform;

    protected:
        GameObject *parent = nullptr;
        std::vector<GameObject *> children = std::vector<GameObject *>();

        virtual void OnLocalTransformChanged();
        virtual void OnParentTransformChanged();

        virtual void OnGlobalTransformChanged();

    public:
        const GameObjectType type = UNKNOWN;
        GameObjectTransform transform;

        GameObject(Transform t);
        GameObject();

        virtual ~GameObject();

        //GameObject Copy();

        size_t SetParent(GameObject *new_parent, bool save_global_pos = true);
        Transform GetParentGlobalTransform();
        Transform GetGlobalTransform();
};

#endif