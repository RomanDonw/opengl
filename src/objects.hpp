#include "utils.hpp"

#include "objects/ShaderProgram.hpp"
#include "objects/Transform.hpp"
#include "objects/GameObject.hpp"

#include "objects/Texture.hpp"
#include "objects/Mesh.hpp"
#include "objects/Surface.hpp"
#include "objects/Entity.hpp"
#include "objects/Camera.hpp"

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