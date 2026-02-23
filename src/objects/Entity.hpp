#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "../glm.hpp"

#include <vector>

#include "GameObject.hpp"
#include "Transform.hpp"
#include "ShaderProgram.hpp"
#include "Surface.hpp"

struct
{
    bool fogEnabled;
    float fogStartDistance;
    float fogEndDistance;
    glm::vec3 fogColor;
} typedef FogRenderSettings;

class Entity : public GameObject
{
    public:
        const GameObjectType type = ENTITY;

        bool enableRender = true;
        glm::vec4 color = glm::vec4(1.0f);
        std::vector<Surface> surfaces = std::vector<Surface>();

        Entity(Transform t);
        Entity();

        ~Entity();

        void Render(ShaderProgram *sp, const glm::mat4 *view, const glm::mat4 *projection, Transform *cameraTransform, const FogRenderSettings *fogRenderSettings);
};

#endif