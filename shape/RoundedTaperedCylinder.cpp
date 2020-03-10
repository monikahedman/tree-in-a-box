#include "RoundedTaperedCylinder.h"

RoundedTaperedCylinder::RoundedTaperedCylinder(int parameter1, int parameter2)
    : Shape(parameter1, parameter2),
      m_circle(std::make_unique<Circle>())
{
}

RoundedTaperedCylinder::~RoundedTaperedCylinder()
{
}

/*
 * Implements Shape's pure virtual function, which sets the member variable
 * m_coordinates in the base class to a std::vector<float> of vertices and
 * normals that can be used as vertex data.
 */
void RoundedTaperedCylinder::generateVBOCoords() {
    // checks that parameter doesn't go below certain bounds
    if (m_parameter2 < 3) {
        setParameter2(3);
    }
    // first clears m_coordinates of old values and reserves enough space for new values
    m_coordinates.clear();
    m_coordinates.reserve(12 * m_parameter2 * (2 * m_parameter1 - 1));
    float p1_reciprocal = 1 / float(m_parameter1);

    float top_radius = 0.4f;
    float magnitude = 1.f / sqrt(1.f + pow((0.5f - top_radius), 2));
    float normal_x = 1.f / magnitude;
    float normal_y = (0.5f - top_radius) / magnitude;

    // gets cone's vertices and normals component by component
    //std::vector<glm::vec3> base = m_circle->generateRings(m_parameter1, m_parameter2, -0.5, 0, 0, 0.5 * p1_reciprocal, glm::vec4(0.0, -1.0, 0.0, 1.0));
    //std::vector<glm::vec3> top = m_circle->generateRings(m_parameter1, m_parameter2, 0.5, 0, top_radius, -top_radius * p1_reciprocal, glm::vec4(0.0, 1.0, 0.0, 1.0));
    std::vector<glm::vec3> top = generateTop(top_radius);
    std::vector<glm::vec3> base = generateBase();
    std::vector<glm::vec3> sides = m_circle->generateRings(m_parameter1, m_parameter2, -0.5, p1_reciprocal, 0.5, -(0.5 - top_radius) * p1_reciprocal, glm::vec4(normal_x, normal_y, 0.0, 1.0));

    m_sideUV.clear();
    int num_vertices = sides.size();
    for (int i = 0; i < num_vertices; i += 2) {
        glm::vec2 uv;
        float theta = atan2(sides[i].z, sides[i].x);
        // float perimeter = (0.5f - (sides[i].y + 0.5f) * (0.5f - top_radius)) * M_PI;
        if (theta < 0) {
            uv.x = -1 * theta / (2.f * M_PI);
        } else {
            uv.x = -1 * theta / (2.f * M_PI) + 1.f;
        }
        uv.y = (sides[i].y + 0.5f);
        m_sideUV.push_back(uv);
    }

    // uses the cone components' vertices to get triangle vertices with their normals and fills m_coordinates with them
    arrayToTriangles(base, 0, (m_parameter1/2.f - 1), 1, (m_parameter1/2.f - 1), (m_parameter2 + 1), 1, m_baseUV);
    //arrayToTriangles(base, 0, (m_parameter1 - 1), 1, (m_parameter1 - 1), (m_parameter2 + 1));
    arrayToTriangles(sides, 0, (m_parameter1 - 1), 0, (m_parameter1 - 1), (m_parameter2 + 1), 1, m_sideUV);
    arrayToTriangles(top, 0, (m_parameter1/2.f - 1), 0, (m_parameter1/2.f), (m_parameter2 + 1), 1, m_topUV);
}

std::vector<glm::vec3> RoundedTaperedCylinder::generateTop(float top_radius) {
    m_topUV.clear();
    std::vector<glm::vec3> vertices;
    vertices.reserve((m_parameter1 + 1) * (m_parameter2 + 1));
    // get angle increments for longitude and latitude
    float phi_increment = M_PI / m_parameter1;
    float theta_increment = (2 * M_PI) / m_parameter2;
    // loops to create "rings" of vertices from the "north pole" of the sphere to the "south pole"
    for (int i = m_parameter1 / 2.f; i <= m_parameter1; i++) {
        for (int j = 0; j <= m_parameter2; j++) {
            // gets spherical coordinate that is quickly converted to Cartesian
            glm::vec3 vertex = sphericalToCartesian(glm::vec3(0.5, M_PI - (phi_increment * i), theta_increment * j));
            glm::vec3 old_vertex = vertex;
            // adds the vertex and its normal (which for a sphere is just the vertex vector normalized) to the return vector
            vertex = glm::vec3(1.f, 0.1f, 1.f) * vertex * (top_radius / 0.5f) + glm::vec3(0.f, 0.5f, 0.f);
            vertices.push_back(vertex);
            vertices.push_back(glm::normalize(vertex));
            glm::vec2 uv;
            float phi = asin(old_vertex.y / 0.5f);
            uv.y = ((phi / M_PI) + 0.5f);
            if (uv.y == 0 || uv.y ==1) {
                uv.x = 0.5f;
            } else {
                float theta = atan2(old_vertex.z, old_vertex.x);
                if (theta < 0) {
                    uv.x = -1 * theta / (2 * M_PI);
                } else {
                    uv.x = (-1 * theta / (2 * M_PI)) + 1;
                }
            }
            m_topUV.push_back(uv);
        }
    }
    return vertices;
}

std::vector<glm::vec3> RoundedTaperedCylinder::generateBase() {
    m_baseUV.clear();
    std::vector<glm::vec3> vertices;
    vertices.reserve((m_parameter1 + 1) * (m_parameter2 + 1));
    // get angle increments for longitude and latitude
    float phi_increment = M_PI / m_parameter1;
    float theta_increment = (2 * M_PI) / m_parameter2;
    // loops to create "rings" of vertices from the "north pole" of the sphere to the "south pole"
    for (int i = 0; i <= m_parameter1 / 2.f; i++) {
        for (int j = 0; j <= m_parameter2; j++) {
            // gets spherical coordinate that is quickly converted to Cartesian
            glm::vec3 vertex = sphericalToCartesian(glm::vec3(0.5, M_PI - (phi_increment * i), theta_increment * j));
            glm::vec3 old_vertex = vertex;
            // adds the vertex and its normal (which for a sphere is just the vertex vector normalized) to the return vector
            vertex = glm::vec3(1.f, 0.1f, 1.f) * vertex + glm::vec3(0.f, -0.5f, 0.f);
            vertices.push_back(vertex);
            vertices.push_back(glm::normalize(vertex));
            glm::vec2 uv;
            float phi = asin(old_vertex.y / 0.5f);
            uv.y = ((phi / M_PI) + 0.5f);
            if (uv.y == 0 || uv.y ==1) {
                uv.x = 0.5f;
            } else {
                float theta = atan2(old_vertex.z, old_vertex.x);
                if (theta < 0) {
                    uv.x = -1 * theta / (2 * M_PI);
                } else {
                    uv.x = (-1 * theta / (2 * M_PI)) + 1;
                }
            }
            m_baseUV.push_back(uv);
        }
    }
    return vertices;
}

glm::vec3 RoundedTaperedCylinder::sphericalToCartesian(glm::vec3 spherical) {
    float x_val = spherical.x * sin(spherical.y) * cos(spherical.z);
    float y_val = spherical.x * cos(spherical.y);
    float z_val = spherical.x * sin(spherical.y) * sin(spherical.z);
    return glm::vec3(x_val, y_val, z_val);
}
