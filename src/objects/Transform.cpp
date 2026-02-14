#include "Transform.hpp"

Transform::Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl)
{
    position = pos;
    rotation = rot;
    scale = scl;
    UpdateCache();
}

Transform::Transform(glm::vec3 pos, glm::vec3 rot)
{
    position = pos;
    rotation = Utils::wrapangles(rot);
    UpdateCache();
}

Transform::Transform(glm::vec3 pos)
{
    position = pos;
    UpdateCache();
}

Transform::Transform() { UpdateCache(); }

Transform Transform::Copy() { return *this; }

void Transform::UpdateCache()
{
    rotation = Utils::wrapangles(rotation);
    rot_quat = glm::quat(rotation);
    rot_mat = glm::toMat4(rot_quat);

    front = rot_quat * glm::vec3(0.0f, 0.0f, -1.0f);
    up = rot_quat * glm::vec3(0.0f, 1.0f, 0.0f);
    right = rot_quat * glm::vec3(1.0f, 0.0f, 0.0f);
}

bool Transform::IsCacheLocked() { return lock_cache; }
void Transform::SetLockCache(bool lock) { lock_cache = lock; }
void Transform::LockCache() { lock_cache = true; }
void Transform::UnlockCache() { lock_cache = false; }

glm::vec3 Transform::GetPosition() { return position; }

glm::vec3 Transform::GetRotation() { return rotation; }
glm::quat Transform::GetRotationQuaternion() { return rot_quat; }
glm::mat4 Transform::GetRotationMatrix() { return rot_mat; }

glm::vec3 Transform::GetScale() { return scale; }

glm::vec3 Transform::GetFront() { return front; }
glm::vec3 Transform::GetUp() { return up; }
glm::vec3 Transform::GetRight() { return right; }

glm::mat4 Transform::GetTransformationMatrix() { return glm::scale(glm::translate(glm::mat4(1), position) * rot_mat, scale); }

void Transform::SetPosition(glm::vec3 v) { position = v; callback_onchange(); }
void Transform::SetRotation(glm::vec3 v) { rotation = v; updatecache(); }
void Transform::SetScale(glm::vec3 v) { scale = v; callback_onchange(); }

void Transform::Translate(glm::vec3 v) { position += v; callback_onchange(); }
void Transform::Rotate(glm::vec3 v) { rotation += v; updatecache(); }
void Transform::Scale(glm::vec3 v) { scale += v; callback_onchange(); }


void Transform::callback_onchange() { for (std::function<void (Transform *)> callback : onTransformChangeCallbacks) callback(this); }