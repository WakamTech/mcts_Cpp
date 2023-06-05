#ifndef __NODE_HPP__
#define __NODE_HPP__

#include "State.hpp"
#include "Model.hpp"
class Node
{

public:
    Node(Node *parent, State *state, const Move *move, Net net);
    ~Node();

    bool terminal;
    Net net;
    unsigned int size;
    unsigned int number_of_simulations;
    double score; // e.g. number of wins (could be int but double is more general if we use evaluation functions)

    State *state;     // current state
    const Move *move; // move to get here from parent node's state
    std::vector<Node *> *children;
    Node *parent;
    std::vector<Move *> *untried_actions;
    void backpropagate(double w, int n);

    bool is_fully_expanded() const;
    bool is_terminal() const;
    const Move *get_move() const;
    unsigned int get_size() const;
    void expand();

    void rollout();
    Node *select_best_child(double c) const;

    Node *advance_tree(const Move *m);
    const State *get_current_state() const;
    void print_stats() const;
    double calculate_winrate(bool player1turn) const;
};

#endif //__NODE_HPP__