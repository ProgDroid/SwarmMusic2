/**
 * A musical scale made up of attractors
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#ifndef SCALE_H_
#define SCALE_H_

#include <vector>

#include <Attractor.h>

const int MAJOR      = 0;
const int MINOR      = 1;
const int PENTATONIC = 2;
const int BLUES      = 3;

const std::vector<std::pair<int, std::vector<int>>> intervals {
    {MAJOR,      {2, 4, 5, 7, 9, 11, 12}},
    {MINOR,      {2, 3, 5, 7, 8, 10, 12}},
    {PENTATONIC, {3, 5, 7, 10, 12}},
    {BLUES,      {3, 5, 6, 7, 10, 12}}
};

const std::vector<std::pair<int, std::vector<int>>> tones {
    {MAJOR,      {1, 2, 3, 4, 5, 6, 7, 1}},
    {MINOR,      {1, 2, 3, 4, 5, 6, 7, 1}},
    {PENTATONIC, {1, 3, 4, 5, 7, 1}},
    {BLUES,      {1, 3, 4, 5, 5, 7, 1}}
};

class Scale
{
private:
    int root;
    int type;

    std::vector<int> pitches;
    std::vector<int> scaleTones;

public:
    Scale(int root, int type);

    std::vector<int> getPitches() const;
    void setPitches(int root, int type);

    std::vector<int> getTones() const;
    void setTones(int type);

    int getRoot() const;
    int getType() const;
};

#endif
