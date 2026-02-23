#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "../glm.hpp"

#include <string>

class Transform
{
    protected:
        glm::vec3 position = glm::vec3(0.0f);
        glm::quat rotation = glm::quat(glm::vec3(0, 0, 0));
        glm::vec3 scale = glm::vec3(1.0f);

        void wrapscale();

        // == callbacks ==
        virtual void OnTransformChanged();

    public:
        Transform(glm::vec3 pos, glm::quat rot, glm::vec3 scl);
        Transform(glm::vec3 pos, glm::quat rot);
        Transform(glm::vec3 pos);
        Transform();

        virtual ~Transform();

        Transform Copy();

        std::string ToString();

        // ================================

        glm::vec3 GetPosition();
        glm::quat GetRotation();
        glm::mat4 GetRotationMatrix();
        glm::vec3 GetScale();

        // ================================

        glm::vec3 GetFront();
        glm::vec3 GetUp();
        glm::vec3 GetRight();

        // ================================

        glm::mat4 GetTransformationMatrix();

        // ================================

        void SetPosition(glm::vec3 v);
        void SetRotation(glm::quat q);
        void SetScale(glm::vec3 v);

        // ================================

        void Translate(glm::vec3 v);
        void Rotate(glm::quat q);
        void Scale(glm::vec3 v);

        // ================================

        Transform LocalToGlobal(const Transform *origin);
        Transform LocalToGlobal(Transform origin);

        Transform GlobalToLocal(const Transform *origin);
        Transform GlobalToLocal(Transform origin);

        // ================================

        Transform *operator=(Transform other);
};


#endif