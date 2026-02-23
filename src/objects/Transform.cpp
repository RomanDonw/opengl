#include "Transform.hpp"

#include <sstream>

// === PRIVATE ===

void Transform::wrapscale()
{
    if (scale.x <= 0) scale.x = 1;
    if (scale.y <= 0) scale.y = 1;
    if (scale.z <= 0) scale.z = 1;
}

void Transform::OnTransformChanged() {}

// === PUBLIC ===

Transform::Transform(glm::vec3 pos, glm::quat rot, glm::vec3 scl)
{
    position = pos;
    rotation = rot;
    scale = scl;

    wrapscale();
    OnTransformChanged();
}

Transform::Transform(glm::vec3 pos, glm::quat rot)
{
    position = pos;
    rotation = rot;
    OnTransformChanged();
}

Transform::Transform(glm::vec3 pos)
{
    position = pos;
    OnTransformChanged();
}

Transform::Transform() { OnTransformChanged(); }

Transform::~Transform() {}

Transform Transform::Copy() { return *this; }

std::string Transform::ToString()
{
    std::ostringstream ss;
    glm::vec3 rot = glm::eulerAngles(rotation);
    ss << "{pos:[" << position.x << "; " << position.y << "; " << position.z << "], rot(euler):[" << rot.x << "; " << rot.y << "; " << rot.z << "], ";
    ss << "scl:[" << scale.x << "; " << scale.y << "; " << scale.z << "]}";
    return ss.str();
}

// ================================

glm::vec3 Transform::GetPosition() { return position; }
glm::quat Transform::GetRotation() { return rotation; }
glm::mat4 Transform::GetRotationMatrix() { return glm::toMat4(rotation); }
glm::vec3 Transform::GetScale() { return scale; }

// ================================

glm::vec3 Transform::GetFront() { return rotation * glm::vec3(0, 0, -1); }
glm::vec3 Transform::GetUp() { return rotation * glm::vec3(0, 1, 0); }
glm::vec3 Transform::GetRight() { return rotation * glm::vec3(1, 0, 0); }

// ================================

glm::mat4 Transform::GetTransformationMatrix()
{ return glm::scale(glm::translate(glm::mat4(1), position) * GetRotationMatrix(), scale); }

// ================================

void Transform::SetPosition(glm::vec3 v) { position = v; OnTransformChanged(); }
void Transform::SetRotation(glm::quat q) { rotation = q; OnTransformChanged(); }
void Transform::SetScale(glm::vec3 v) { scale = v; wrapscale(); OnTransformChanged(); }

// ================================

void Transform::Translate(glm::vec3 v) { position += v; OnTransformChanged(); }
void Transform::Rotate(glm::quat q) { rotation = q * rotation; OnTransformChanged(); }
void Transform::Scale(glm::vec3 v) { scale += v; wrapscale(); OnTransformChanged(); }

// ================================

Transform Transform::LocalToGlobal(const Transform *origin)
{
    glm::vec3 pos = origin->rotation * position + origin->position;
    glm::quat rot = origin->rotation * rotation;
    glm::vec3 scl = origin->scale * scale;
    return Transform(pos, rot, scl);
}
Transform Transform::LocalToGlobal(Transform origin) { return LocalToGlobal(&origin); }

Transform Transform::GlobalToLocal(const Transform *origin)
{
    glm::vec3 pos = glm::inverse(origin->rotation) * (position - origin->position);
    glm::quat rot = glm::inverse(origin->rotation) * rotation;
    glm::vec3 scl = scale / origin->scale;
    return Transform(pos, rot, scl);
}
Transform Transform::GlobalToLocal(Transform origin) { return GlobalToLocal(&origin); }

// ================================

Transform *Transform::operator=(Transform other)
{
    position = other.position;
    rotation = other.rotation;
    scale = other.scale;
    wrapscale();

    OnTransformChanged();

    return this;
}