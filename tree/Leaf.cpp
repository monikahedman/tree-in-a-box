#include "Leaf.h"

Leaf::Leaf():
    m_renderer(std::make_unique<OpenGLShape>())
{

}

void Leaf::loadRenderer() {
    std::vector<GLfloat> vertex_data = {0.f, 0.f, 0.f, \
                                      0.f, 0.f, 1.f, \
                                      0.f, 0.f, \
                                      1.f, 0.f, 0.f, \
                                      0.f, 0.f, 1.f, \
                                      1.f, 0.f, \
                                      0.f, 1.f, 0.f, \
                                      0.f, 0.f, 1.f, \
                                      0.f, 1.f};
    m_renderer->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_renderer->setAttribute(ShaderAttrib::NORMAL, 3, 3 * sizeof (GLfloat), VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_renderer->setAttribute(ShaderAttrib::TEXCOORD0, 2, 6 * sizeof (GLfloat), VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_renderer->setVertexData(&vertex_data[0], 24, VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLES, 3);
    m_renderer->buildVAO();
}

/*
 * The shape's renderer draws the shape.
 */
void Leaf::renderShape() {
    m_renderer->draw();
}
