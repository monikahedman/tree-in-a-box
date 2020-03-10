#ifndef ROUNDEDTAPEREDCYLINDER_H
#define ROUNDEDTAPEREDCYLINDER_H

#include "Shape.h"
#include "Circle.h"

class RoundedTaperedCylinder
    : public Shape
{
public:
    RoundedTaperedCylinder(int parameter1, int parameter2);
    virtual ~RoundedTaperedCylinder() override;

    // implements Shape's pure virtual function
    void generateVBOCoords() override;

private:
    // Circle member variable that will call its own methods to generate parts
    std::unique_ptr<Circle> m_circle;

    std::vector<glm::vec3> generateTop(float top_radius);
    std::vector<glm::vec3> generateBase();
    glm::vec3 sphericalToCartesian(glm::vec3 spherical);
};

#endif // ROUNDEDTAPEREDCYLINDER_H
