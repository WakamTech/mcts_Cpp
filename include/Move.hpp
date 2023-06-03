#ifndef __MOVE_HPP__
#define __MOVE_HPP__

#include <vector>
#include <queue>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <string>

#define STARTING_NUMBER_OF_CHILDREN 256
struct Move
{
    int x, y;
    int player;
    Move(int x, int y, int p) : x(x), y(y), player(p) {}
    bool operator==(const Move &other) const
    {
        const Move &o = (const Move &)other; // Note: Casting necessary
        return x == o.x && y == o.y && player == o.player;
    }
    std::string sprint() const { return "Not implemented"; } // and optionally this
};

#endif //__MOVE_HPP__