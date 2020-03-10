#ifndef CYLINDER_H
#define CYLINDER_H

#include "Shape.h"
#include "Circle.h"

class Cylinder
    : public Shape
{
public:
    Cylinder(int parameter1, int parameter2);
    virtual ~Cylinder() override;

    // implements Shape's pure virtual function
    void generateVBOCoords() override;

private:
    // Circle member variable that will call its own methods to generate cylinder parts
    std::unique_ptr<Circle> m_circle;

};

#endif // CYLINDER_H
