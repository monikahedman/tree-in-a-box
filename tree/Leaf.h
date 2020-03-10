#ifndef LEAF_H
#define LEAF_H

#include <memory>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "gl/openglshape.h"
#include "gl/shaders/ShaderAttribLocations.h"

class Leaf
{
public:
    Leaf();

    void loadRenderer();
    void renderShape();
    std::unique_ptr<OpenGLShape> m_renderer;
};

#endif // LEAF_H
