#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

#include "glm.hpp"

/*#define min(x, y) x < y ? x : y
#define max(x, y) x > y ? x : y*/

namespace Utils
{
    float sign(float x);
    double sign(double x);
    
    /*
        Returns angles of given radius-vector relative to Z+ axis.
        Examples:
            vector {0; 0; 1} gives {0; 0; 0} angles,
            {0; 0; -1} gives {0; PI; 0} angles,
            {1; 0; 0} gives {0; PI / 2; 0} angles.
    */
    glm::vec3 angles(glm::vec3 v, float roll = 0);
    
    glm::vec3 normalize(glm::vec3 v);
    glm::vec3 wrapangles(glm::vec3 euler);
    std::string tostring(glm::vec3 v);
}

#endif