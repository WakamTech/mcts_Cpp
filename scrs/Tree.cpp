#include "Tree.hpp"

Tree::Tree(State *starting_state, Net net)
{
    this->net = net;
    assert(starting_state != NULL);
    root = new Node(NULL, starting_state, NULL, net);
}
Tree::~Tree()
{
    delete root;
}
Node *Tree::select(double c)
{
    Node *node = root;
    while (!node->is_terminal())
    {
        if (!node->is_fully_expanded())
        {
            return node;
        }
        else
        {
            node = node->select_best_child(c);
        }
    }
    return node;
} // select child node to expand according to tree policy (UCT)
Node *Tree::select_best_child() // select the most promising child of the root node
{
    return root->select_best_child(0.0);
}
void Tree::grow_tree(int max_iter, double max_time_in_seconds)
{
    Node *node;
    double dt;
#ifdef DEBUG
    std::cout << "Growing tree..." << std::endl;
#endif
    time_t start_t, now_t;
    time(&start_t);
    for (int i = 0; i < max_iter; i++)
    {
        // select node to expand according to tree policy
        node = select();
        // expand it (this will perform a rollout and backpropagate the results)
        node->expand();
        // check if we need to stop
        time(&now_t);
        dt = difftime(now_t, start_t);
        if (dt > max_time_in_seconds)
        {
#ifdef DEBUG
            cout << "Early stopping: Made " << (i + 1) << " iterations in " << dt << " seconds." << endl;
#endif
            break;
        }
    }
#ifdef DEBUG
    time(&now_t);
    dt = difftime(now_t, start_t);
    cout << "Finished in " << dt << " seconds." << endl;
#endif
}

void Tree::advance_tree(const Move *move) // if the move is applicable advance the tree, else start over
{
    Node *old_root = root;
    root = root->advance_tree(move);
    delete old_root;
}
unsigned int Tree::get_size() const
{
    return root->get_size();
}
const State *Tree::get_current_state() const
{
    return root->get_current_state();
}
void Tree::print_stats() const
{
    root->print_stats();
}