#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <vector>

#include "../utils.hpp"
#include "../glm.hpp"

class Transform
{
  private:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    bool lock_cache = false;
    void updatecache();

    // cache:
    glm::quat rot_quat;
    glm::mat4 rot_mat;

    glm::vec3 front, up, right;

    //void callback_onchange();

    void OnTransformUpdated();

  public:
    //std::vector<std::function<void (Transform *)>> onTransformChangeCallbacks = std::vector<std::function<void (Transform *)>>();

    Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl);
    Transform(glm::vec3 pos, glm::vec3 rot);
    Transform(glm::vec3 pos);
    Transform();

    Transform Copy();

    bool IsCacheLocked();
    void SetLockCache(bool lock);
    void LockCache();
    void UnlockCache();

    void UpdateCache();

    glm::vec3 GetPosition();
    
    glm::vec3 GetRotation();
    glm::quat GetRotationQuaternion();
    glm::mat4 GetRotationMatrix();

    glm::vec3 GetScale();

    glm::vec3 GetFront();
    glm::vec3 GetUp();
    glm::vec3 GetRight();

    glm::mat4 GetTransformationMatrix();

    void SetPosition(glm::vec3 v);
    void SetRotation(glm::vec3 v);
    void SetScale(glm::vec3 v);

    void Translate(glm::vec3 v);
    void Rotate(glm::vec3 v);
    void Scale(glm::vec3 v);
};


#endif