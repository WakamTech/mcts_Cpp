#ifndef __TREE_HPP__
#define __TREE_HPP__

#include "State.hpp"
#include "Node.hpp"
#include "Model.hpp"
class Tree
{
    Node *root;

public:
    Tree(State *starting_state, Net net);
    ~Tree();
    Net net;
    Node *select(double c = 1.41);         // select child node to expand according to tree policy (UCT)
    Node *select_best_child() ;
    void grow_tree(int max_iter, double max_time_in_seconds);

    void advance_tree(const Move *move) ;
    unsigned int get_size() const;
    const State *get_current_state() const;
    void print_stats() const;
};

#endif //__TREE_HPP__