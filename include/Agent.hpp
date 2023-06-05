#ifndef __AGENT_HPP__
#define __AGENT_HPP__

#include "Tree.hpp"
#include "Model.hpp"
class Agent
{ // example of an agent based on the MCTS_tree. One can also use the tree directly.

public:
    Agent(State *starting_state, Net net, int max_iter = 100000, int max_seconds = 30);
    ~Agent();

    Tree *tree;
    int max_iter, max_seconds;
    Net net;
    const Move *genmove(const Move *enemy_move);
    const State *get_current_state() const;
    void feedback() const;
};


#endif