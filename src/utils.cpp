#include "utils.hpp"

namespace Utils
{
    glm::vec3 normalize(glm::vec3 v)
    {
        if (glm::length(v) == 0.0f) return glm::vec3(0.0f);
        return glm::normalize(v);
    }

    float sign(float x)
    {
        if (x > 0) return 1;
        else if (x < 0) return -1;
        return 0;
    }

    double sign(double x)
    {
        if (x > 0) return 1;
        else if (x < 0) return -1;
        return 0;
    }

    glm::vec3 angles(glm::vec3 v, float roll)
    {
        float len = glm::length(v);
        if (len == 0) return glm::vec3(0.0f);

        return glm::vec3(glm::asin(v.y / len), glm::atan(v.x, v.z), roll);
    }

    std::string tostring(glm::vec3 v)
    { return "{" + std::to_string(v.x) + "; " + std::to_string(v.y) + "; " + std::to_string(v.z) + "}"; }

    glm::vec3 wrapangles(glm::vec3 euler) { return glm::vec3(fmod(euler.x, 360.0f), fmod(euler.y, 360.0f), fmod(euler.z, 360.0f)); }
}
