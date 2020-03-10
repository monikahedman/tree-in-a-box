#include "Cone.h"

Cone::Cone(int parameter1, int parameter2)
    : Shape(parameter1, parameter2),
      m_circle(std::make_unique<Circle>())
{
}

Cone::~Cone()
{
}

/*
 * Implements Shape's pure virtual function, which sets the member variable
 * m_coordinates in the base class to a std::vector<float> of vertices and
 * normals that can be used as vertex data.
 */
void Cone::generateVBOCoords() {
    // checks that parameter doesn't go below certain bounds
    if (m_parameter2 < 3) {
        setParameter2(3);
    }
    // first clears m_coordinates of old values and reserves enough space for new values
    m_coordinates.clear();
    m_coordinates.reserve(12 * m_parameter2 * (2 * m_parameter1 - 1));
    float p1_reciprocal = 1 / float(m_parameter1);
    // gets cone's vertices and normals component by component
    std::vector<glm::vec3> base = m_circle->generateRings(m_parameter1, m_parameter2, -0.5, 0, 0, 0.5 * p1_reciprocal, glm::vec4(0.0, -1.0, 0.0, 1.0));
    std::vector<glm::vec3> sides = m_circle->generateRings(m_parameter1, m_parameter2, -0.5, p1_reciprocal, 0.5, -0.5 * p1_reciprocal, glm::vec4(2.0/sqrt(5), 1.0/sqrt(5), 0.0, 1.0));
    // uses the cone components' vertices to get triangle vertices with their normals and fills m_coordinates with them
    arrayToTriangles(base, 0, (m_parameter1 - 1), 1, (m_parameter1 - 1), (m_parameter2 + 1), 0, std::vector<glm::vec2>());
    arrayToTriangles(sides, 0, (m_parameter1 - 2), 0, (m_parameter1 - 1), (m_parameter2 + 1), 0, std::vector<glm::vec2>());
}
