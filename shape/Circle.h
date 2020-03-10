#ifndef CIRCLE_H
#define CIRCLE_H

#include <vector>
#include <math.h>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

class Circle
{
public:
    Circle();

    // generic function that generates vertices in rings (can represent cone/cylinder caps and sides)
    std::vector<glm::vec3> generateRings(int parameter1, int parameter2, float start_y, float y_increment, float start_r, float r_increment, glm::vec4 normal);
};

#endif // CIRCULE_H
