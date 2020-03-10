#ifndef CUBE_H
#define CUBE_H

#include "Shape.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include <memory>
#include <vector>

class Cube : public Shape {
public:
    Cube(int parameter1, int parameter2);
    Cube(int parameter1, int parameter2, bool reverse);
    virtual ~Cube() override;

    // implements Shape's pure virtual function
    void generateVBOCoords() override;

private:
    /**
     * @brief m_reverse If this is true, the cube is drawn inside-out.
     */
    bool m_reverse;

    // generates vertices and normals for one face of the cube
    std::vector<glm::vec3> makeFace(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 normal);
    // vector storing the 3 corners of interest and normal vector for each cube face
    std::vector<glm::vec3> m_corners;
};

#endif // CUBE_H
