#include "Circle.h"

Circle::Circle()
{
}

/*
 * Generates vertices and normals for a generic shape composed of rings that can differ
 * in radius and y_value (height off XY plane) by certain increments. Normals are generated
 * by rotating some starting normal vector around the y-axis.
 *
 * @param parameter1 {int} Determines how many rings there are.
 * @param parameter2 {int} Determines how many sides each ring has.
 * @param start_y {int} Starting height off XY plane.
 * @param y_increment {int} Height to be incremented by for each ring generated.
 * @param start_r {int} Staring radius of ring.
 * @param r_increment {int} Radius length to be incremented by for each ring generated.
 * @param normal {glm::vec4} Starting normal vector on XY plane (in homogeneous coordinates).
 *
 * @return {std::vector<glm::vec3>} Vector of glm::vec3's of each vertex (with normal) in all the rings.
 */
std::vector<glm::vec3> Circle::generateRings(int parameter1, int parameter2, float start_y, float y_increment, float start_r, float r_increment, glm::vec4 normal) {
    std::vector<glm::vec3> rings;
    rings.reserve(2 * (parameter1 + 1) * (parameter2 + 1));
    // angle increment determines how many sides a ring has
    float angle_increment = (M_PI * 2) / float(parameter2);
    // rotation matrix for normal vectors
    glm::mat4 R = glm::rotate(-1 * float(M_PI * 2) / float(parameter2), glm::vec3(0.0, 1.0, 0.0));
    for (int i = 0; i <= parameter1; i++) {
        glm::vec4 curr_normal = normal;
        for (int j = 0; j <= parameter2; j++) {
            // gets values for cylindrical coordinate of vertex
            float y_val = start_y + (i * y_increment);
            float angle = j * angle_increment;
            float radius = start_r + i * r_increment;
            // adds vertex and normal to rings vector
            rings.push_back(glm::vec3(radius * cos(angle), y_val, radius * sin(angle)));
            rings.push_back(curr_normal.xyz());
            // rotates the current normal vector
            curr_normal = R * curr_normal;
        }
    }
    return rings;
}
