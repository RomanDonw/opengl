#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <vector>

#include "../glm.hpp"
#include "Transform.hpp"

enum
{
    UNKNOWN = 0,
    ENTITY = 1,
    CAMERA = 2,
    AUDIOSOURCE = 3
} typedef GameObjectType;

class GameObject
{
  private:
    GameObject *parent = nullptr;
    std::vector<GameObject *> children = std::vector<GameObject *>();

  public:
    const GameObjectType type = UNKNOWN;
    Transform transform = Transform();

    //GameObject(Transform t) { transform = t; }
    GameObject(Transform t);
    GameObject();

    ~GameObject();

    GameObject Copy();

    void SetParent(GameObject *new_parent);
    glm::mat4 GetParentGlobalTransformationMatrix();
};

#endif