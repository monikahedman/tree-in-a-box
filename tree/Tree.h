#ifndef TREE_H
#define TREE_H

#include "CS123SceneData.h"
#include "LSystem.h"
#include "Settings.h"
#include <QList>
#include <memory>
#include <stack>
#include "glm/gtx/transform.hpp"
#include <glm/gtc/quaternion.hpp>
#include <iostream>

class Tree
{
public:
    Tree();

    void createTree();
    CS123ScenePrimitive m_branch;
    QList<glm::mat4x4> m_transformations;
    QList<glm::mat4x4> m_leaf_transforms;
    glm::vec4 m_turtle_pos;
    glm::vec4 m_turtle_dir;
    std::stack<std::vector<glm::vec4>> m_turtle_states;
    std::stack<std::vector<float>> m_branch_states;

    std::unique_ptr<LSystem> m_lsystem;

    float m_length;
    float m_radius;
    float m_angle;
    float m_leaf_size;

    float m_stem_length;
    float m_stem_radius;

private:
    void tree();
    void primitive();
    void decreaseRadius(int *already_turn);
    void decreaseLength();
    glm::mat4x4 makeLeafRotation(glm::vec3 direction);
    glm::mat4x4 makeRotationMatrix();
    float sampleFromRange(float center, float low_range, float high_range);
};

#endif // TREE_H
