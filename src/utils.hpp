#ifndef UTILS_HPP
#define UTILS_HPP

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

    glm::vec3 angles(glm::vec3 v, float roll = 0)
    {
        float len = glm::length(v);
        if (len == 0) return glm::vec3(0.0f);

        return glm::vec3(glm::acos(v.z / len), glm::atan(v.y, v.x), roll);
    }

    /*
    // "Max" function overloads.

    unsigned int Max(unsigned int a, unsigned int b)
    {
        if (a > b) return a;
        return b;
    }

    int Max(int a, int b)
    {
        if (a > b) return a;
        return b;
    }

    float Max(float a, float b)
    {
        if (a > b) return a;
        return b;
    }

    double Max(double a, double b)
    {
        if (a > b) return a;
        return b;
    }

    // "Min" function overloads.

    unsigned int Min(unsigned int a, unsigned int b)
    {
        if (a < b) return a;
        return b;
    }

    int Min(int a, int b)
    {
        if (a < b) return a;
        return b;
    }

    float Min(float a, float b)
    {
        if (a < b) return a;
        return b;
    }

    double Min(double a, double b)
    {
        if (a < b) return a;
        return b;
    }
    */
}

#endif