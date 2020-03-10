#include "Shape.h"
#include <QDebug>

Shape::Shape(int parameter1, int parameter2) :
    m_coordinates(std::vector<float>()),
    m_parameter1(parameter1),
    m_parameter2(parameter2),
    m_renderer(std::make_unique<OpenGLShape>())

{
}

Shape::~Shape()
{
}

/*
 * The coordinates are generated and the renderer is set up for drawing.
 */
void Shape::loadRenderer() {
    generateVBOCoords();
    m_renderer->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_renderer->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_renderer->setAttribute(ShaderAttrib::TEXCOORD0, 2, 24, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_renderer->setVertexData(&m_coordinates[0], m_coordinates.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLES, m_coordinates.size()/8);
    m_renderer->buildVAO();
}

/*
 * The shape's renderer draws the shape.
 */
void Shape::renderShape() {
    m_renderer->draw();
}

/*
 * Sets m_parameter1 to a new value.
 *
 * @param parameter1 {int} New value m_parameter1 should be set to.
 */
void Shape::setParameter1(int parameter1) {
    m_parameter1 = parameter1;
}

/*
 * Sets m_parameter2 to a new value.
 *
 * @param parameter2 {int} New value m_parameter2 should be set to.
 */
void Shape::setParameter2(int parameter2) {
    m_parameter2 = parameter2;
}

/*
 * Takes in a std::vector<glm::vec3> of vertices paired with their normals, as well as parameters that relate to
 * how to loop through it (depending on what shape component it is). Looping through the vector will result in
 * m_coordinates being appended with triangle vertices with this structure:
 * {triangle1_x, triangle1_y, triangle1_z, normal1_x, normal1_y, normal1_z, triangle2_x, triangle2_y, triangle2_z, normal2_x, normal2_y, normal2_z,...}
 *
 * @param src {std::vector<glm::vec3>} Vector of vertices and normals to loop through.
 * @param l_row_start {int} Starting row to loop through for the triangles oriented one way (if src was thought of as a 2D array).
 * @param l_row_end {int} Ending row to loop through for the triangles oriented one way (if src was thought of as a 2D array).
 * @param r_row_start {int} Starting row to loop through for the triangles oriented the other way (if src was thought of as a 2D array).
 * @param r_row_end {int} Ending row to loop through for the triangles oriented the other way (if src was thought of as a 2D array).
 * @param num_cols {int} Number of columns src would have if it was thought of as a 2D array.
 */
void Shape::arrayToTriangles(std::vector<glm::vec3> src, int l_row_start, int l_row_end, int r_row_start, int r_row_end, int num_cols, int tapered, std::vector<glm::vec2> UVs) {
    // index offsets to get vertices for triangles oriented one way
    int index_change1 = 2 * num_cols;
    int index_change2 = 2;
    // row start and end correspond to the ones triangles oriented one way
    int row_start = l_row_start;
    int row_end = l_row_end;
    // loops twice because there are two type of triangle orientations
    for (int k = 0; k < 2; k++) {
        for (int i = row_start; i <= row_end; i++) {
            for (int j = 0; j <= (num_cols - 2); j++) {
                // These values are needed to calculate UVs.
                // The UV calculations are kind of a mess but ¯\_(ツ)_/¯.
                float end = float(row_end);
                float start = float(row_start);
                float u1 = (float(i) - start) / (end - start + 1.f);
                float u2 = (float(i + 1) - start) / (end - start + 1.f);
                float v1 = float(j) / float(num_cols - 1);
                float v2 = float(j + 1) / float(num_cols - 1);

                // src is not actually a 2D array, so the indices need to be converted to 1D index
                int index = 2 * ((i * num_cols) + j);
                // adds vertex 1 of triangle and its normal to m_coordinates
                addVec3ToVector(src[index]);
                addVec3ToVector(src[index + 1]);

                // Adds UVs.
                if (tapered == 1) {
                    m_coordinates.push_back(UVs[index / 2].x);
                    m_coordinates.push_back(UVs[index / 2].y);
                } else {
                    m_coordinates.push_back(u1);
                    m_coordinates.push_back(v1);
                }

                index += index_change1;
                // adds vertex 2 of triangle and its normal to m_coordinates
                addVec3ToVector(src[index]);
                addVec3ToVector(src[index + 1]);

                // Adds UVs.
                if (tapered == 1) {
                    m_coordinates.push_back(UVs[index / 2].x);
                    m_coordinates.push_back(UVs[index / 2].y);
                } else {
                    m_coordinates.push_back(u2);
                    m_coordinates.push_back(k == 1 ? v2 : v1);
                }

                index += index_change2;
                // adds vertex 3 of triangle and its normal to m_coordinates
                addVec3ToVector(src[index]);
                addVec3ToVector(src[index + 1]);

                // Adds UVs.
                if (tapered == 1) {
                    m_coordinates.push_back(UVs[index / 2].x);
                    m_coordinates.push_back(UVs[index / 2].y);
                } else {
                    m_coordinates.push_back(k == 1 ? u1 : u2);
                    m_coordinates.push_back(v2);
                }
            }
        }
        // index offsets to get vertices for triangles oriented the other way
        index_change1 = 2 * num_cols + 2;
        index_change2 = -2 * num_cols;
        // row start and end correspond to the ones triangles oriented the other way
        row_start = r_row_start;
        row_end = r_row_end;
    }
}

/*
 * Appends components of a glm::vec3 to m_coordinates, a std::vect<float>.
 *
 * @param vec3 {glm::vec3} Vector of values to be appended to m_coordinates.
 */
void Shape::addVec3ToVector(glm::vec3 vec3) {
    m_coordinates.push_back(vec3.x);
    m_coordinates.push_back(vec3.y);
    m_coordinates.push_back(vec3.z);
}
