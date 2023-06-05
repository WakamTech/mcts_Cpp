#include "Agent.hpp"

Agent::Agent(State *starting_state, Net net , int max_iter, int max_seconds)
    : max_iter(max_iter), net(net), max_seconds(max_seconds)
{
    tree = new Tree(starting_state, net);
}
Agent::~Agent()
{
    delete tree;
}
const Move *Agent::genmove(const Move *enemy_move)
{
    if (enemy_move != NULL)
    {
        tree->advance_tree(enemy_move);
    }
    // If game ended from opponent move, we can't do anything
    if (tree->get_current_state()->is_terminal())
    {
        return NULL;
    }
#ifdef DEBUG
    std::cout << "___ DEBUG ______________________" << std::endl
              << "Growing tree..." << std::endl;
#endif
    tree->grow_tree(max_iter, max_seconds);
#ifdef DEBUG
    std::cout << "Tree size: " << tree->get_size() << std::endl
              << "________________________________" << std::endl;
#endif
    Node *best_child = tree->select_best_child();
    if (best_child == NULL)
    {
        std::cerr << "Warning: Tree root has no children! Possibly terminal node!" << std::endl;
        return NULL;
    }
    const Move *best_move = best_child->get_move();
    tree->advance_tree(best_move);
    return best_move;
}
const State *Agent::get_current_state() const
{
    return tree->get_current_state();
}
void Agent::feedback() const { tree->print_stats(); }
