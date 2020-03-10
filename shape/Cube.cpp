#include "Cube.h"

Cube::Cube(int parameter1, int parameter2) :
    Shape(parameter1, parameter2),
    m_reverse(false),
    m_corners({glm::vec3(-0.5, 0.5, 0.5),  glm::vec3(0.5, 0.5, 0.5),    glm::vec3(-0.5, -0.5, 0.5),  glm::vec3(0.0, 0.0, 1.0),
               glm::vec3(0.5, 0.5, -0.5),  glm::vec3(-0.5, 0.5, -0.5),  glm::vec3(0.5, -0.5, -0.5),  glm::vec3(0.0, 0.0, -1.0),
               glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0.5, 0.5, -0.5),   glm::vec3(-0.5, 0.5, 0.5),   glm::vec3(0.0, 1.0, 0.0),
               glm::vec3(0.5, -0.5, -0.5), glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0.5, -0.5, 0.5),   glm::vec3(0.0, -1.0, 0.0),
               glm::vec3(0.5, 0.5, 0.5),   glm::vec3(0.5, 0.5, -0.5),   glm::vec3(0.5, -0.5, 0.5),   glm::vec3(1.0, 0.0, 0.0),
               glm::vec3(-0.5, 0.5, -0.5), glm::vec3(-0.5, 0.5, 0.5),   glm::vec3(-0.5, -0.5, -0.5), glm::vec3(-1.0, 0.0, 0.0)}) {}

Cube::Cube(int parameter1, int parameter2, bool reverse) :
    Shape(parameter1, parameter2),
    m_reverse(reverse),
    m_corners({glm::vec3(-0.5, 0.5, 0.5),  glm::vec3(0.5, 0.5, 0.5),    glm::vec3(-0.5, -0.5, 0.5),  glm::vec3(0.0, 0.0, 1.0),
               glm::vec3(0.5, 0.5, -0.5),  glm::vec3(-0.5, 0.5, -0.5),  glm::vec3(0.5, -0.5, -0.5),  glm::vec3(0.0, 0.0, -1.0),
               glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0.5, 0.5, -0.5),   glm::vec3(-0.5, 0.5, 0.5),   glm::vec3(0.0, 1.0, 0.0),
               glm::vec3(0.5, -0.5, -0.5), glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0.5, -0.5, 0.5),   glm::vec3(0.0, -1.0, 0.0),
               glm::vec3(0.5, 0.5, 0.5),   glm::vec3(0.5, 0.5, -0.5),   glm::vec3(0.5, -0.5, 0.5),   glm::vec3(1.0, 0.0, 0.0),
               glm::vec3(-0.5, 0.5, -0.5), glm::vec3(-0.5, 0.5, 0.5),   glm::vec3(-0.5, -0.5, -0.5), glm::vec3(-1.0, 0.0, 0.0)}) {}

Cube::~Cube() {}

/*
 * Generates vertices and normals for a face of a cube on the plane determined by the
 * 3 corner vertices passed in.
 *
 * @param c1 {glm::vec3} Top left corner of face.
 * @param c2 {glm::vec3} Top right corner of face.
 * @param c3 {glm::vec3} Bottom left corner of face.
 * @param normal {glm::vec3} Height to be incremented by for each ring generated.
 *
 * @return {std::vector<glm::vec3>} Vector of glm::vec3's of each vertex (with normal) on a face.
 */
std::vector<glm::vec3> Cube::makeFace(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 normal) {
    std::vector<glm::vec3> vertices;
    vertices.reserve((m_parameter1 + 1) * (m_parameter1 + 1));
    // get increments in "left" and "down" directions as vec3's
    glm::vec3 i_increment = (c3 - c1) / float(m_parameter1);
    glm::vec3 j_increment = (c2 - c1) / float(m_parameter1);
    for (int i = 0; i <= m_parameter1; i++) {
        for (int j = 0; j <= m_parameter1; j++) {
            // gets vertex from current offset from starting corner c1
            glm::vec3 vertex = c1 + float(i) * i_increment + float(j) * j_increment;
            vertices.push_back(m_reverse ? -vertex : vertex);
            // all vertices on the same face have the same normal
            vertices.push_back(normal);
        }
    }
    return vertices;
}

/*
 * Implements Shape's pure virtual function, which sets the member variable
 * m_coordinates in the base class to a std::vector<float> of vertices and
 * normals that can be used as vertex data.
 */
void Cube::generateVBOCoords() {
    // first clears m_coordinates of old values and reserves enough space for new values
    m_coordinates.clear();
    m_coordinates.reserve(72 * m_parameter1 * m_parameter1);
    // loops through each face
    for (int i = 0; i < 6; i++) {
        // gets corners and normal determining current face
        glm::vec3 c1 = m_corners[4 * i];
        glm::vec3 c2 = m_corners[4 * i + 1];
        glm::vec3 c3 = m_corners[4 * i + 2];
        glm::vec3 normal = m_corners[4 * i + 3];
        // gets vertices and normals for face, then adds them to m_coordinates after processing
        std::vector<glm::vec3> vertices = makeFace(c1, c2, c3, normal);
        arrayToTriangles(vertices, 0, (m_parameter1 - 1), 0, (m_parameter1 - 1), (m_parameter1 + 1), 0, std::vector<glm::vec2>());
    }
}
