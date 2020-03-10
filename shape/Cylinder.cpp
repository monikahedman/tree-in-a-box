#include "Cylinder.h"

Cylinder::Cylinder(int parameter1, int parameter2)
    : Shape(parameter1, parameter2),
      m_circle(std::make_unique<Circle>())
{
}

Cylinder::~Cylinder()
{
}

/*
 * Implements Shape's pure virtual function, which sets the member variable
 * m_coordinates in the base class to a std::vector<float> of vertices and
 * normals that can be used as vertex data.
 */
void Cylinder::generateVBOCoords() {
    // checks that parameters don't go below certain bounds
    if (m_parameter2 < 3) {
        setParameter2(3);
    }

    // I'm not sure exactly how your math works, so I'm applying a constant factor to the number of vertices.
    const int WITH_UV = 8;
    const int WITHOUT_UV = 6;

    // first clears m_coordinates of old values and reserves enough space for new values
    m_coordinates.clear();
    m_coordinates.reserve(12 * m_parameter2 * (3 * m_parameter1 - 1) * WITH_UV / WITHOUT_UV);
    float p1_reciprocal = 1 / float(m_parameter1);
    // gets cylinder's vertices and normals component by component
    std::vector<glm::vec3> base = m_circle->generateRings(m_parameter1, m_parameter2, -0.5, 0, 0, 0.5 * p1_reciprocal, glm::vec4(0.0, -1.0, 0.0, 1.0));
    std::vector<glm::vec3> top = m_circle->generateRings(m_parameter1, m_parameter2, 0.5, 0, 0.5, -0.5 * p1_reciprocal, glm::vec4(0.0, 1.0, 0.0, 1.0));
    std::vector<glm::vec3> sides = m_circle->generateRings(m_parameter1, m_parameter2, -0.5, p1_reciprocal, 0.5, 0, glm::vec4(1.0, 0.0, 0.0, 1.0));
    // uses the cylinder components' vertices to get triangle vertices with their normals and fills m_coordinates with them
    arrayToTriangles(base, 0, (m_parameter1 - 1), 1, (m_parameter1 - 1), (m_parameter2 + 1), 0, std::vector<glm::vec2>());
    arrayToTriangles(top, 0, (m_parameter1 - 2), 0, (m_parameter1 - 1), (m_parameter2 + 1), 0, std::vector<glm::vec2>());
    arrayToTriangles(sides, 0, (m_parameter1 - 1), 0, (m_parameter1 - 1), (m_parameter2 + 1), 0, std::vector<glm::vec2>());
}
