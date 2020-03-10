#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <memory>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "gl/openglshape.h"
#include "gl/shaders/ShaderAttribLocations.h"

class Shape {
public:
    Shape(int parameter1, int parameter2);
    virtual ~Shape();

    // loads the renderer so it can draw
    void loadRenderer();
    // renderer draws shape
    void renderShape();

    // setters for m_parameter1 and m_parameter2
    void setParameter1(int parameter1);
    void setParameter2(int parameter2);
    // pure virtual function to be implemented in subclasses
    // (gets vertices and normals, then processes them and adds them to m_coordinates)
    virtual void generateVBOCoords() = 0;

    // stores floats to be used for vertex data
    std::vector<float> m_coordinates;

protected:
    // processes an array of vertices and normals so that it can be used as vertex data
    void arrayToTriangles(std::vector<glm::vec3> src, int l_row_start, int l_row_end, int r_row_start, int r_row_end, int num_cols, int tapered, std::vector<glm::vec2> UVs);
    // adds components of a glm::vec3 to member variable m_coordinates
    void addVec3ToVector(glm::vec3 vec3);

    // parameter member variables
    int m_parameter1;
    int m_parameter2;

    // renderer
    std::unique_ptr<OpenGLShape> m_renderer;

    std::vector<glm::vec2> m_topUV;
    std::vector<glm::vec2> m_baseUV;
    std::vector<glm::vec2> m_sideUV;
};

#endif // SHAPE_H
