/**
 * A musical scale made up of attractors
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#include <Scale.h>

Scale::Scale(int root, int type) : root(root), type(type) {
    setPitches(root, type);
    setTones(type);
}

std::vector<int> Scale::getPitches() const {
    return this->pitches;
}

void Scale::setPitches(int root, int type) {
    std::pair<int, std::vector<int>> scale = intervals.at(type);

    if (scale.first == type) {
        int size = scale.second.size();

        // intervals + root
        pitches.reserve(size + 1);
        pitches.push_back(root);

        for (int i = 0; i < size; ++i) {
            pitches.push_back(root + scale.second.at(i));
        }
    }
}

std::vector<int> Scale::getTones() const {
    return this->scaleTones;
}

void Scale::setTones(int type) {
    std::pair<int, std::vector<int>> scale = tones.at(type);

    if (scale.first == type) {
        int size = scale.second.size();

        scaleTones.reserve(size);

        for (int i = 0; i < size; ++i) {
            scaleTones.push_back(scale.second.at(i));
        }
    }
}
