#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"
#include <math.h>

class Sphere
    : public Shape
{
public:
    Sphere(int parameter1, int parameter2);
    virtual ~Sphere() override;

    // implements Shape's pure virtual function
    void generateVBOCoords() override;

    std::vector<glm::vec2> m_uvs;

private:
    glm::vec2 findUV(glm::vec3 vertex);
    // generates all the vertices and normals for a sphere
    std::vector<glm::vec3> generateVertices();
    // converts a spherical coordinate to Cartesian
    glm::vec3 sphericalToCartesian(glm::vec3 spherical);
};

#endif // SPHERE_H
