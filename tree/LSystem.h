#ifndef LSYSTEM_H
#define LSYSTEM_H

#include <map>
#include <string>
#include <vector>
#include <iterator>
#include <stack>

class LSystem
{
public:
    LSystem();

    void generateSequence();
    void expandTree();
    int randomSample(float main_prob, float other_prob, float num_choices);

    int m_iters;

    float m_length;
    float m_angle;
    float m_radius;
    float m_length_decay;
    float m_radius_decay;
    std::map<std::string, std::vector<std::string>> m_rules;

    std::string m_tree;
    //std::stack<std> states;
};

#endif // LSYSTEM_H
