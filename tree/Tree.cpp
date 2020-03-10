#include "Tree.h"

Tree::Tree():
    m_branch(CS123ScenePrimitive()),
    m_lsystem(std::make_unique<LSystem>())
{
    m_branch.type = PrimitiveType::PRIMITIVE_CYLINDER;
    m_branch.material = CS123SceneMaterial();
    m_branch.material.cAmbient = glm::vec4(0.7f, 0.65f, 0.55f, 1.f);
}

void Tree::createTree() {
    tree();
}

void Tree::tree() {
    m_transformations.clear();
    m_leaf_transforms.clear();
    m_turtle_pos = glm::vec4(0.f, -6.f, 0.f, 1.f);
    m_turtle_dir = glm::vec4(0.f, 1.f, 0.f, 0.f);

    m_lsystem->generateSequence();
    std::string tree_string = m_lsystem->m_tree;

    m_length = settings.branchLength;
    m_radius = settings.branchRadius;
    m_angle = settings.branchAngle;
    m_leaf_size = settings.leafSize;

    m_stem_length = 0.07f;
    m_stem_radius = 0.03f;

    int already_turn = 0;

    for (int i = 0; i < tree_string.length(); i++) {
        char current_char = tree_string[i];

        switch (current_char) {
            case 'F': {
                already_turn = 0;
                float actual_length = sampleFromRange(m_length, m_length * 0.15f, m_length * 0.15f);
                float actual_radius = sampleFromRange(m_radius, m_radius * 0.1f, m_radius * 0.1f);;
                glm::vec3 cylinder_center = (m_turtle_pos + m_turtle_dir * actual_length * 0.5f).xyz();
                glm::mat4x4 translate = glm::translate(cylinder_center);
                glm::mat4x4 rotate = makeRotationMatrix();
                glm::mat4x4 scale = glm::scale(glm::vec3(actual_radius / 2.f, actual_length, actual_radius / 2.f));
                m_turtle_pos += m_turtle_dir * actual_length;
                m_transformations.push_back(translate * rotate * scale);
                decreaseLength();
                break;
            }
            case 'L': {
                glm::mat4x4 translate = glm::translate(glm::vec3());
                glm::mat4x4 rotate = makeLeafRotation(m_turtle_dir.xyz());
                glm::mat4x4 scale = glm::scale(glm::vec3(m_leaf_size * 0.3f, m_leaf_size * 0.3f, 1.f));
                m_leaf_transforms.push_back(translate * rotate * scale);
                break;
            }
            case '[': {
                already_turn = 0;
                std::vector<glm::vec4> turtle_state;
                turtle_state.reserve(2);
                turtle_state.push_back(m_turtle_pos);
                turtle_state.push_back(m_turtle_dir);
                m_turtle_states.push(turtle_state);
                std::vector<float> branch_state;
                branch_state.push_back(m_length);
                branch_state.push_back(m_radius);
                m_branch_states.push(branch_state);
                break;
            }
            case ']': {
                // draw leaf
                glm::vec3 direction = glm::normalize(m_turtle_dir.xyz() + 2.f * glm::vec3((rand() % 100 - 50) / 100.f, (rand() % 100 - 90) / 100.f, (rand() % 100 - 50) / 100.f));
                glm::mat4x4 rotate = makeLeafRotation(direction);
                glm::mat4x4 translate = glm::translate(m_turtle_pos.xyz());
                float random = (rand() % 50 + 75) / 100.f;
                glm::mat4x4 scale = glm::scale(glm::vec3(m_leaf_size * 0.3f * random, m_leaf_size * 0.3f * random, 1.f));
                m_leaf_transforms.push_back(translate * rotate * scale);

                already_turn = 0;
                std::vector<glm::vec4> turtle_state = m_turtle_states.top();
                m_turtle_states.pop();
                m_turtle_pos = turtle_state[0];
                m_turtle_dir = turtle_state[1];
                std::vector<float> branch_state = m_branch_states.top();
                m_branch_states.pop();
                m_length = branch_state[0];
                m_radius = branch_state[1];
                break;
            }
            case '/': {
                // roll CCW
                float actual_angle = sampleFromRange(m_angle, m_angle * 0.15f, m_angle * 0.15f);
                glm::mat4x4 rotate = glm::rotate(-1.f * actual_angle, glm::vec3(1.f, 0.f, 0.f));
                m_turtle_dir = glm::normalize(rotate * m_turtle_dir);
                decreaseRadius(&already_turn);
                break;
            }
            case '\\': {
                // roll CW
                float actual_angle = sampleFromRange(m_angle, m_angle * 0.15f, m_angle * 0.15f);
                glm::mat4x4 rotate = glm::rotate(actual_angle, glm::vec3(1.f, 0.f, 0.f));
                m_turtle_dir = glm::normalize(rotate * m_turtle_dir);
                decreaseRadius(&already_turn);
                break;
            }
            case '+': {
                // turn right
                float actual_angle = sampleFromRange(m_angle, m_angle * 0.15f, m_angle * 0.15f);
                glm::mat4x4 rotate = glm::rotate(-1.f * actual_angle, glm::vec3(0.f, 0.f, 1.f));
                m_turtle_dir = glm::normalize(rotate * m_turtle_dir);
                decreaseRadius(&already_turn);
                break;
            }
            case '-': {
                // turn left
                float actual_angle = sampleFromRange(m_angle, m_angle * 0.15f, m_angle * 0.15f);
                glm::mat4x4 rotate = glm::rotate(actual_angle, glm::vec3(0.f, 0.f, 1.f));
                m_turtle_dir = glm::normalize(rotate * m_turtle_dir);
                decreaseRadius(&already_turn);
                break;
            }
            case '&': {
                // pitch down
                float actual_angle = sampleFromRange(m_angle, m_angle * 0.15f, m_angle * 0.15f);
                glm::mat4x4 rotate = glm::rotate(actual_angle, glm::vec3(0.f, 1.f, 0.f));
                m_turtle_dir = glm::normalize(rotate * m_turtle_dir);
                decreaseRadius(&already_turn);
                break;
            }
            case '^': {
                // pitch up
                float actual_angle = sampleFromRange(m_angle, m_angle * 0.15f, m_angle * 0.15f);
                glm::mat4x4 rotate = glm::rotate(-1.f * actual_angle, glm::vec3(0.f, 1.f, 0.f));
                m_turtle_dir = glm::normalize(rotate * m_turtle_dir);
                decreaseRadius(&already_turn);
                break;
            }
            default:
                // Dummy variable should be ignored
                already_turn = 0;
                break;
        }
    }
}

void Tree::primitive() {
    m_transformations.clear();
    m_transformations.push_back(glm::mat4x4());
}

void Tree::decreaseRadius(int *already_turn) {
    if (*already_turn == 0) {
        m_radius *= 0.92f;
        *already_turn = 1;
    }
}

void Tree::decreaseLength() {
    m_length *= 0.95f;
}

glm::mat4x4 Tree::makeRotationMatrix() {
    glm::vec3 up_vector = glm::vec3(0.f, 1.f, 0.f);
    float cosTheta = glm::dot(up_vector, m_turtle_dir.xyz());
    glm::vec3 rotation_axis = glm::cross(up_vector, m_turtle_dir.xyz());

    float s = sqrt((1 + cosTheta) * 2.f);
    float invs = 1.f / s;

    glm::quat quaternion = glm::quat(s * 0.5f, rotation_axis.x * invs, rotation_axis.y * invs, rotation_axis.z * invs);
    return glm::mat4_cast(quaternion);
}

glm::mat4x4 Tree::makeLeafRotation(glm::vec3 direction) {
    glm::vec3 up_vector = glm::vec3(0.f, 1.f, 0.f);
    float cosTheta = glm::dot(up_vector, direction);
    glm::vec3 rotation_axis = glm::cross(up_vector, direction);

    float s = sqrt((1 + cosTheta) * 2.f);
    float invs = 1.f / s;

    glm::quat quaternion = glm::quat(s * 0.5f, rotation_axis.x * invs, rotation_axis.y * invs, rotation_axis.z * invs);
    return glm::mat4_cast(quaternion);
}

float Tree::sampleFromRange(float center, float low_range, float high_range) {
    float total_range = low_range + high_range;
    float random = (rand() % 100) / 100.f;
    return random * total_range + center - low_range;
}
