#ifndef CONE_H
#define CONE_H

#include "Shape.h"
#include "Circle.h"

class Cone
    : public Shape
{
public:
    Cone(int parameter1, int parameter2);
    virtual ~Cone() override;

    // implements Shape's pure virtual function
    void generateVBOCoords() override;

private:
    // Circle member variable that will call its own methods to generate cylinder parts
    std::unique_ptr<Circle> m_circle;

};

#endif // CONE_H
