#include "Sphere.h"

Sphere::Sphere(int parameter1, int parameter2)
    : Shape(parameter1, parameter2)

{
}

Sphere::~Sphere()
{
}

/*
 * Generates each vertex and its associated normal as glm::vec3's for a sphere.
 *
 * @return {std::vector<glm::vec3>} The vector of vertices and normals structured
 * as {vertex_1, normal_1, vertex_2, normal_2, ..., vertex_n, normal_n}.
 */
std::vector<glm::vec3> Sphere::generateVertices() {
    std::vector<glm::vec3> vertices;
    vertices.reserve(2 * (m_parameter1 + 1) * (m_parameter2 + 1));
    // get angle increments for longitude and latitude
    float phi_increment = M_PI / m_parameter1;
    float theta_increment = (2 * M_PI) / m_parameter2;
    // loops to create "rings" of vertices from the "north pole" of the sphere to the "south pole"
    for (int i = 0; i <= m_parameter1; i++) {
        for (int j = 0; j <= m_parameter2; j++) {
            // gets spherical coordinate that is quickly converted to Cartesian
            glm::vec3 vertex = sphericalToCartesian(glm::vec3(0.5, M_PI - (phi_increment * i), theta_increment * j));
            // adds the vertex and its normal (which for a sphere is just the vertex vector normalized) to the return vector
            vertices.push_back(vertex);
            vertices.push_back(glm::normalize(vertex));
        }
    }
    return vertices;
}

glm::vec2 Sphere::findUV(glm::vec3 vertex) {
    glm::vec2 uv;
    float phi = asin(vertex.y / 0.5f);
    uv.y = ((phi / M_PI) + 0.5f);
    if (uv.y == 0 || uv.y == 1) {
        uv.x = 0.5f;
    } else {
        float theta = atan2(vertex.z, vertex.x);
        if (theta < 0) {
            uv.x = -1 * theta / (2 * M_PI);
        } else {
            uv.x = (-1 * theta / (2 * M_PI)) + 1;
        }
    }
    return uv;
}

/*
 * Takes in a spherical coordinate and returns it as a Cartesian coordinate.
 *
 * @param spherical {glm::vec3} vec3 in spherical coordinates to be converted.
 * @return {glm::vec3} vec3 Cartesian coordinate after conversion.
 */
glm::vec3 Sphere::sphericalToCartesian(glm::vec3 spherical) {
    float x_val = spherical.x * sin(spherical.y) * cos(spherical.z);
    float y_val = spherical.x * cos(spherical.y);
    float z_val = spherical.x * sin(spherical.y) * sin(spherical.z);
    return glm::vec3(x_val, y_val, z_val);
}

/*
 * Implements Shape's pure virtual function, which sets the member variable
 * m_coordinates in the base class to a std::vector<float> of vertices and
 * normals that can be used as vertex data.
 */
void Sphere::generateVBOCoords() {
    // checks that parameters don't go below certain bounds
    if (m_parameter1 < 2) {
        setParameter1(2);
    }
    if (m_parameter2 < 3) {
        setParameter2(3);
    }
    // first clears m_coordinates of old values and reserves enough space for new values
    m_coordinates.clear();
    m_coordinates.reserve(12 * m_parameter2 * (m_parameter1 - 1));
    // gets sphere's vertices and normals
    std::vector<glm::vec3> vertices = generateVertices();

    m_uvs.clear();
    int num_vertices = vertices.size();
    for (int i = 0; i < num_vertices; i += 2) {
        m_uvs.push_back(findUV(vertices[i]));
    }

    // uses the sphere's vertices to get triangle vertices with their normals and fills m_coordinates with them
    arrayToTriangles(vertices, 0, (m_parameter1 - 2), 1, (m_parameter1 - 1), (m_parameter2 + 1), 1, m_uvs);
}
