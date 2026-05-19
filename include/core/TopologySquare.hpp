#pragma once

#include <string>

struct TopologySquare
{
    int id = -1;

    int logicalRow = -1;
    int logicalCol = -1;

    float displayX = 0.0f;
    float displayY = 0.0f;

    bool active = false;
    std::string label;
};