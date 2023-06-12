#include "Node.hpp"

Node::Node(Node *parent, State *state, const Move *move, Net net)
    : parent(parent), state(state), move(move), net(net), score(0.0), number_of_simulations(0), size(0)
{
    children = new std::vector<Node *>();
    children->reserve(STARTING_NUMBER_OF_CHILDREN);
    untried_actions = state->actions_to_try();
    terminal = state->is_terminal();
}

Node::~Node()
{
    delete state;
    delete move;
    for (auto *child : *children)
    {
        delete child;
    }
    delete children;
    while (!untried_actions->empty())
    {
        delete untried_actions->back(); // if a move is here then it is not a part of a child node and needs to be deleted here
        untried_actions->pop_back();
    }
    delete untried_actions;
}

void Node::backpropagate(double w, int n)
{
    score += w;
    number_of_simulations += n;
    if (parent != NULL)
    {
        parent->size++;
        parent->backpropagate(w, n);
    }
}
bool Node::is_fully_expanded() const
{
    return is_terminal() || untried_actions->empty();
}
bool Node::is_terminal() const
{
    return terminal;
}
const Move *Node::get_move() const
{
    return move;
}
unsigned int Node::get_size() const
{
    return size;
}

void Node::expand()
{
    if (is_terminal())
    {              // can legitimately happen in end-game situations
        rollout(); // keep rolling out, eventually causing UCT to pick another node to expand due to exploration
        return;
    }
    else if (is_fully_expanded())
    {
        std::cerr << "Warning: Cannot expanded this node any more!" << std::endl;
        return;
    }
    // get next untried action
    Move *next_move = NULL;

    // untried_actions->pop_back();
    auto my_new_state = state->state_to_model_input(); // remove it
    auto my_new_state_to_tensor = torch::from_blob(my_new_state.data(), {1, 9});

    auto policy = net->forward(my_new_state_to_tensor);
    auto value = torch::argmax(policy).item<int>();

    Move *next_movee = new Move(value / 3, value % 3, state->get_turn()); // get value
    if (find(untried_actions->begin(), untried_actions->end(), next_movee) != untried_actions->end())
    {
        next_move = next_movee;
    }
    else
    {
        next_move = untried_actions->back();
        untried_actions->pop_back();
    }

    State *next_state = state->next_state(next_move);
    // build a new MCTS node from it
    Node *new_node = new Node(this, next_state, next_move, net);
    // rollout, updating its stats
    new_node->rollout();
    // add new node to tree
    children->push_back(new_node);
}

void Node::rollout()
{
    double w = state->rollout();
    backpropagate(w, 1);
}
Node *Node::select_best_child(double c) const
{
    if (children->empty())
        return NULL;
    else if (children->size() == 1)
        return children->at(0);
    else
    {
        double uct, max = -1;
        Node *argmax = NULL;
        for (auto *child : *children)
        {
            double winrate = child->score / ((double)child->number_of_simulations);
            // If its the opponent's move apply UCT based on his winrate i.e. our loss rate.   <-------
            if (!state->player1_turn())
            {
                winrate = 1.0 - winrate;
            }
            if (c > 0)
            {
                uct = winrate +
                      c * sqrt(log((double)this->number_of_simulations) / ((double)child->number_of_simulations));
            }
            else
            {
                uct = winrate;
            }
            if (uct > max)
            {
                max = uct;
                argmax = child;
            }
        }
        return argmax;
    }
}
Node *Node::advance_tree(const Move *m)
{
    // Find child with this m and delete all others
    Node *next = NULL;
    for (auto *child : *children)
    {
        if (*(child->move) == *(m))
        {
            next = child;
        }
        else
        {
            delete child;
        }
    }
    // remove children from queue so that they won't be re-deleted by the destructor when this node dies (!)
    this->children->clear();
    // if not found then we have to create a new node
    if (next == NULL)
    {
        // Note: UCT may lead to not fully explored tree even for short-term children due to terminal nodes being chosen
        std::cout << "INFO: Didn't find child node. Had to start over." << std::endl;
        State *next_state = state->next_state(m);
        next = new Node(NULL, next_state, NULL, net);
    }
    else
    {
        next->parent = NULL; // make parent NULL
        // IMPORTANT: m and next->move can be the same here if we pass the move from select_best_child()
        // (which is what we will typically be doing). If not then it's the caller's responsibility to delete m (!)
    }
    // return the next root
    return next;
}
const State *Node::get_current_state() const
{
    return state;
}
void Node::print_stats() const
{
#define TOPK 10
    if (number_of_simulations == 0)
    {
        std::cout << "Tree not expanded yet" << std::endl;
        return;
    }
    std::cout << "___ INFO _______________________" << std::endl
              << "Tree size: " << size << std::endl
              << "Number of simulations: " << number_of_simulations << std::endl
              << "Branching factor at root: " << children->size() << std::endl
              << "Chances of P1 winning: " << std::setprecision(4) << 100.0 * (score / number_of_simulations) << "%" << std::endl;
    // sort children based on winrate of player's turn for this node (!)
    if (state->player1_turn())
    {
        std::sort(children->begin(), children->end(), [](const Node *n1, const Node *n2)
                  { return n1->calculate_winrate(true) > n2->calculate_winrate(true); });
    }
    else
    {
        std::sort(children->begin(), children->end(), [](const Node *n1, const Node *n2)
                  { return n1->calculate_winrate(false) > n2->calculate_winrate(false); });
    }
    // print TOPK of them along with their winrates
    std::cout << "Best moves:" << std::endl;
    for (int i = 0; i < children->size() && i < TOPK; i++)
    {
        std::cout << "  " << i + 1 << ". " << children->at(i)->move->sprint() << "  -->  "
                  << std::setprecision(4) << 100.0 * children->at(i)->calculate_winrate(state->player1_turn()) << "%" << std::endl;
    }
    std::cout << "________________________________" << std::endl;
}

double Node::calculate_winrate(bool player1turn) const
{
    if (player1turn)
    {
        return score / number_of_simulations;
    }
    else
    {
        return 1.0 - score / number_of_simulations;
    }
}
