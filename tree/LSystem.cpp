#include "LSystem.h"

LSystem::LSystem():
    m_iters(4),
    m_tree("X")
{
    m_rules.insert(std::pair<std::string, std::vector<std::string>>("F", {"F", "F&F^", "F+F-", "F-F+", "F^F&", "F/F\\", "F\\F/"}));
    m_rules.insert(std::pair<std::string, std::vector<std::string>>("X", {"F[-&X]+F[-/X]+F[^\\X]+[+\\X]+X", \
                                                                          "F[+^X]-F[+\\X]-[&/X]-F[^\\X]&X", \
                                                                          "F[/&X]\\[/^X]\\F[&+X]\\F[^X]\\X", \
                                                                          "F[\\-X]/F[+X]/F[&-X]/[^\\X]+X", \
                                                                          "F[/+X]&F[^-X]&F[^/X]&F[+\\X]^X",
                                                                          "F[&-X]^F[\\X]^F[++X]^F[/-X]&X"}));
}

void LSystem::generateSequence() {
    m_tree.clear();
    m_tree = "[X][X][X]";
    int num_iterations = m_iters;
    for (int i = 0; i < num_iterations; i++) {
        expandTree();
    }
}

void LSystem::expandTree() {
    int str_len = m_tree.length();
    std::string current_char = "";
    int increment = 0;
    for (int i = 0; i < str_len; i++) {
        current_char = m_tree.at(i);
        std::map<std::string, std::vector<std::string>>::iterator iter;
        for (iter = m_rules.begin(); iter != m_rules.end(); iter++) {
            if (current_char.compare(iter->first) == 0) {
                std::vector<std::string> replacements = iter->second;
                if (replacements.size() == 1) {
                    m_tree.replace(i, 1, replacements[0]);
                    increment = replacements[0].length() - 1;
                } else {
                    float other_prob = 1.f / replacements.size();
                    float main_prob = 1 - (other_prob * (replacements.size() - 1));
                    int index = randomSample(main_prob, other_prob, replacements.size());
                    m_tree.replace(i, 1, replacements[index]);
                    increment = replacements[index].length() - 1;
                }
                str_len += increment;
                i += increment;
            }
        }
    }
}

int LSystem::randomSample(float main_prob, float other_prob, float num_choices) {
    float random = (rand() % 100) / 100.f;
    float prob_acc = main_prob;
    int index;
    for (index = 0; index < num_choices; index++) {
        if (random < prob_acc) {
            break;
        }
        prob_acc += other_prob;
    }
    return index;
}
