#include "utils.hpp"

#include "objects/ShaderProgram.hpp"
#include "objects/Transform.hpp"
#include "objects/GameObject.hpp"

#include "objects/Texture.hpp"
#include "objects/Mesh.hpp"
#include "objects/Surface.hpp"

#include "audio.hpp"


/*
class AABB
{
  private:
    glm::vec3 min = glm::vec3(0.0f);
    glm::vec3 max = glm::vec3(0.0f);

  public:
    AABB(glm::vec3 point1, glm::vec3 point2) { SetBounds(point1, point2); }

    inline AABB Copy() { return *this; }

    inline glm::vec3 GetMinPoint() { return min; }
    inline glm::vec3 GetMaxPoint() { return max; }
    inline glm::vec3 GetSize() { return max - min; }
    inline glm::vec3 GetCenterOffset() { return (min + max) / 2.0f; }

    inline void Translate(glm::vec3 offset) { min += offset; max += offset; }
    void SetBounds(glm::vec3 point1, glm::vec3 point2)
    {
        min = glm::vec3(glm::min(point1.x, point2.x), glm::min(point1.y, point2.y), glm::min(point1.z, point2.z));
        max = glm::vec3(glm::max(point1.x, point2.x), glm::max(point1.y, point2.y), glm::max(point1.z, point2.z));
    }

    inline bool PointIntersects(glm::vec3 point)
    { return (point.x >= min.x && point.y >= min.y && point.z >= min.z) && (point.x <= max.x && point.y <= max.y && point.z <= max.z); }

    bool AABBIntersects(AABB *aabb)
    {
        glm::vec3 b_min = aabb->GetMinPoint();
        glm::vec3 b_max = aabb->GetMaxPoint();
        return (min.x <= b_max.x && min.y <= b_max.y && min.z <= b_max.z) && (max.x >= b_min.x && max.y >= b_min.y && max.z >= b_min.z);
    }
    inline bool AABBIntersects(AABB aabb) { return AABBIntersects(&aabb); }

    inline AABB GetAABBOverlapRegion(AABB *target) { return AABB(glm::max(target->GetMinPoint(), min), glm::min(target->GetMaxPoint(), max)); }
    inline AABB GetAABBOverlapRegion(AABB target) { return GetAABBOverlapRegion(&target); }

    inline glm::vec3 GenRightSideNormal() { return Utils::normalize(glm::vec3(max.x, min.y, min.z)); }
    inline glm::vec3 GenUpSideNormal() { return Utils::normalize(glm::vec3(min.x, max.y, min.z)); }
    inline glm::vec3 GenFrontSideNormal() { return -Utils::normalize(glm::vec3(min.x, min.y, max.z)); } // negative because in OpenGL front is -Z direction instead of +Z.

    glm::vec3 GetAABBPenetration(AABB *target)
    {
        if (!AABBIntersects(target)) return glm::vec3(0.0f);

        glm::vec3 b_min = target->GetMinPoint();
        glm::vec3 b_max = target->GetMaxPoint();

        glm::vec3 ret;
        ret.x = glm::min(max.x - b_min.x, b_max.x - min.x);
        ret.y = glm::min(max.y - b_min.y, b_max.y - min.y);
        ret.z = glm::min(max.z - b_min.z, b_max.z - min.z);
        return ret;
    }

    inline glm::vec3 GetAABBWeightedPenetration(AABB *target) { return (target->GetCenterOffset() - GetCenterOffset()) * GetAABBPenetration(target); }
    
    glm::vec3 GetAABBSingleDirectionPenetration(AABB *target)
    {
        glm::vec3 p = GetAABBPenetration(target);
        glm::vec3 a_center = GetCenterOffset();
        glm::vec3 b_center = target->GetCenterOffset();

        if (p.x > p.y && p.x > p.z) return p * glm::vec3(Utils::sign(a_center.x - b_center.x), 0.0f, 0.0f);
        else if (p.y > p.z) return p * glm::vec3(0.0f, Utils::sign(a_center.y - b_center.y), 0.0f);
        return p * glm::vec3(0.0f, Utils::sign(a_center.z - b_center.z), 0.0f);
    }
};*/

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

    Entity(Transform t) : GameObject(t) {}
    Entity() : GameObject() {}

    ~Entity() {}

    void Render(ShaderProgram *sp, const glm::mat4 *view, const glm::mat4 *projection, Transform *cameraTransform, const FogRenderSettings *fogRenderSettings)
    {
        if (!enableRender) return;

        sp->UseThisProgram();

        sp->SetUniformMatrix4x4("projection", *projection);
        sp->SetUniformMatrix4x4("view", *view);

        sp->SetUniformInteger("texture", 0);
        glActiveTexture(GL_TEXTURE0);

        sp->SetUniformVector3("cameraPosition", cameraTransform->GetPosition());
        sp->SetUniformVector3("cameraRotation", glm::eulerAngles(cameraTransform->GetRotation()));

        sp->SetUniformVector3("cameraFront", cameraTransform->GetFront());
        sp->SetUniformVector3("cameraUp", cameraTransform->GetUp());
        sp->SetUniformVector3("cameraRight", cameraTransform->GetRight());

        sp->SetUniformInteger("fogEnabled", fogRenderSettings->fogEnabled ? GL_TRUE : GL_FALSE);
        sp->SetUniformFloat("fogStartDistance", fogRenderSettings->fogStartDistance);
        sp->SetUniformFloat("fogEndDistance", fogRenderSettings->fogEndDistance);
        sp->SetUniformVector3("fogColor", fogRenderSettings->fogColor);

        for (Surface surface : surfaces)
        {
            if (!surface.enableRender) continue;

            Texture *texture = surface.GetTexture();
            Mesh *mesh = surface.GetMesh();
            if (!(mesh && mesh->HasBuffers() && surface.culling != BothFaces)) continue;

            if (surface.culling == NoCulling) glDisable(GL_CULL_FACE);
            else
            {
                glEnable(GL_CULL_FACE);

                switch (surface.culling)
                {
                    case BackFace:
                        glCullFace(GL_BACK);
                        break;

                    case FrontFace:
                        glCullFace(GL_FRONT);
                        break;
                }
            }

            if (texture && texture->HasTexture())
            {
                sp->SetUniformInteger("hasTexture", GL_TRUE);
                texture->BindTexture();
            }
            else sp->SetUniformInteger("hasTexture", GL_FALSE);

            sp->SetUniformMatrix4x4("model", GetGlobalTransform().GetTransformationMatrix() * surface.transform.GetTransformationMatrix());
            sp->SetUniformVector4("color", color * surface.color);

            mesh->RenderMesh();
        }
    }
};

class Camera : public GameObject
{
  private:
    float neardist = 0.05;//0.1;
    float fardist = 1000;
    float fov = glm::radians(60.0f);
    
  public:
    const GameObjectType type = CAMERA;

    Camera(float _fov, float _neardist, float _fardist) : GameObject()
    {
        fov = _fov;
        neardist = _neardist;
        fardist = _fardist;
    }

    Camera(float _neardist, float _fardist) : GameObject()
    {
        neardist = _neardist;
        fardist = _fardist;
    }

    Camera(float _fov) : GameObject() { fov = _fov; }

    Camera() : GameObject() {}

    inline float GetNearDistance() { return neardist; }
    inline float GetFarDistance() { return fardist; }
    inline float GetFOV() { return fov; }

    inline void SetNearDistance(float _neardist) { neardist = _neardist; }
    inline void SetFarDistance(float _fardist) { fardist = _fardist; }
    inline void SetFOV(float _fov) { fov = _fov; }

    inline glm::mat4 GetViewMatrix()
    { return glm::lookAt(transform.GetPosition(), transform.GetPosition() + transform.GetFront(), transform.GetUp()); }
    inline glm::mat4 GetProjectionMatrix(unsigned int screen_width, unsigned int screen_height)
    { return glm::perspective(fov, (float)screen_width / (float)screen_height, neardist, fardist); }
};