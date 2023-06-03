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
/*
    ##calculate_winner
    ##player_won
    ##change_turn
    ##get_turn
    ##get_winner
    ##is_terminal
    ##next_state
    ##actions_to_try
    ##rollout
*/
class State
{
private:
    /* data */
public:
    State() : turn(-1)
    {
        // initialize board as empty
        for (int i = 0; i < 9; i++)
        {
            board[i / 3][i % 3] = 0;
        }
        // calculate winner
        winner = calculate_winner();
    }
    State(const State &other) : turn(other.turn), winner(other.winner)
    {
        // copy board
        for (int i = 0; i < 9; i++)
        {
            board[i / 3][i % 3] = other.board[i / 3][i % 3];
        }
    }

    int board[3][3]{};

    bool player_won(char player) const
    {
        if (player != 1 and player != -1)
            std::cerr << "Warning: check winner for unknown player" << std::endl;
        for (int i = 0; i < 3; i++)
        {
            if (board[i][0] == player && board[i][1] == player && board[i][2] == player)
                return true;
            if (board[0][i] == player && board[1][i] == player && board[2][i] == player)
                return true;
        }
        return (board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
               (board[0][2] == player && board[1][1] == player && board[2][0] == player);
    }
    int calculate_winner() const
    {
        if (player_won(1))
            return 1;
        else if (player_won(0))
            return 0;
        bool all_taken = true;
        for (int i = 0; i < 9; i++)
        {
            if (board[i / 3][i % 3] == 0)
            {
                all_taken = false;
                break;
            }
        }
        if (all_taken)
            return 2; // draw
        else
            return 0;
    }
    int turn, winner;
    void change_turn() { turn = (turn == 1) ? -1 : 1; }

public:
    int get_turn() const { return turn; }
    int get_winner() const { return winner; }
    bool is_terminal() const
    {
        return winner != 0;
    }
    State *next_state(const Move *move) const
    {
        // Note: We have to manually cast it to its correct type
        Move *m = (Move *)move;
        State *new_state = new State(*this); // create new state from current
        if (new_state->board[m->x][m->y] == 0)
        {
            new_state->board[m->x][m->y] = m->player;          // play move
            new_state->winner = new_state->calculate_winner(); // check again for a winner
            new_state->change_turn();
        }
        else
        {
            std::cerr << "Warning: Illegal move (" << m->x << ", " << m->y << ")" << std::endl;
            return NULL;
        }
        return new_state;
    }

    std::vector<Move *> *actions_to_try() const
    {
        std::vector<Move *> *Q = new std::vector<Move *>();
        for (int i = 0; i < 9; i++)
        {
            if (board[i / 3][i % 3] == 0)
            {
                Q->push_back(new Move(i / 3, i % 3, turn));
            }
        }
        return Q;
    }

    double rollout() const
    {
        if (is_terminal())
            return (winner == 1) ? 1.0 : (winner == 2) ? 0.5
                                                       : 0.0;
        // Simulate a completely random game
        // Note: dequeue is not very efficient for random accesses but vector is not efficient for deletes
        std::vector<int> available;
        for (int i = 0; i < 9; i++)
        {
            if (board[i / 3][i % 3] == 0)
            {
                available.push_back(i);
            }
        }
        long long r;
        int a;
        State *curstate = (State *)this; // TODO: ignore const...
        srand(time(NULL));
        bool first = true;
        do
        {
            if (available.empty())
            {
                std::cerr << "Warning: Ran out of available moves and state is not terminal?";
                return 0.0;
            }
            r = rand() % available.size();
            a = available[r];
            Move move(a / 3, a % 3, curstate->turn);
            available.erase(available.begin() + r); // delete from available moves
            State *old = curstate;
            curstate = (State *)curstate->next_state(&move);
            if (!first)
            {
                delete old;
            }
            first = false;
        } while (!curstate->is_terminal());
        double res = (curstate->winner == 1) ? 1.0 : (curstate->winner == 2) ? 0.5
                                                                             : 0.0;
        delete curstate;
        return res;
    }
    void print() const
    {
        for (int i = 0; i < 9; i++)
        {
            std::cout << board[i / 3][i % 3] << " ";
            if ((i + 1) % 3 == 0)
                std::cout << "\n";
        }
    }
    bool player1_turn() const { return turn == '1'; }
};

class Node
{
    bool terminal;
    unsigned int size;
    unsigned int number_of_simulations;
    double score; // e.g. number of wins (could be int but double is more general if we use evaluation functions)

    State *state;     // current state
    const Move *move; // move to get here from parent node's state
    std::vector<Node *> *children;
    Node *parent;
    std::vector<Move *> *untried_actions;
    void backpropagate(double w, int n)
    {
        score += w;
        number_of_simulations += n;
        if (parent != NULL)
        {
            parent->size++;
            parent->backpropagate(w, n);
        }
    }

public:
    Node(Node *parent, State *state, const Move *move)
        : parent(parent), state(state), move(move), score(0.0), number_of_simulations(0), size(0)
    {
        children = new std::vector<Node *>();
        children->reserve(STARTING_NUMBER_OF_CHILDREN);
        untried_actions = state->actions_to_try();
        terminal = state->is_terminal();
    }
    ~Node()
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

    bool is_fully_expanded() const
    {
        return is_terminal() || untried_actions->empty();
    }
    bool is_terminal() const
    {
        return terminal;
    }
    const Move *get_move() const
    {
        return move;
    }
    unsigned int get_size() const
    {
        return size;
    }

    void expand()
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
        Move *next_move = untried_actions->back(); // get value
        untried_actions->pop_back();               // remove it
        State *next_state = state->next_state(next_move);
        // build a new MCTS node from it
        Node *new_node = new Node(this, next_state, next_move);
        // rollout, updating its stats
        new_node->rollout();
        // add new node to tree
        children->push_back(new_node);
    }

    void rollout()
    {
        double w = state->rollout();
        backpropagate(w, 1);
    }
    Node *select_best_child(double c) const
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
    Node *advance_tree(const Move *m)
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
            next = new Node(NULL, next_state, NULL);
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
    const State *get_current_state() const
    {
        return state;
    }
    void print_stats() const
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
    double calculate_winrate(bool player1turn) const
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
};

class Tree
{
    Node *root;

public:
    Tree(State *starting_state)
    {
        assert(starting_state != NULL);
        root = new Node(NULL, starting_state, NULL);
    }
    ~Tree()
    {
        delete root;
    }
    Node *select(double c = 1.41)
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
    }                         // select child node to expand according to tree policy (UCT)
    Node *select_best_child() // select the most promising child of the root node
    {
        return root->select_best_child(0.0);
    }
    void grow_tree(int max_iter, double max_time_in_seconds)
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

    void advance_tree(const Move *move) // if the move is applicable advance the tree, else start over
    {
        Node *old_root = root;
        root = root->advance_tree(move);
        delete old_root;
    }
    unsigned int get_size() const
    {
        return root->get_size();
    }
    const State *get_current_state() const
    {
        return root->get_current_state();
    }
    void print_stats() const
    {
        root->print_stats();
    }
};

class Agent
{ // example of an agent based on the MCTS_tree. One can also use the tree directly.
    Tree *tree;
    int max_iter, max_seconds;

public:
    Agent(State *starting_state, int max_iter = 100000, int max_seconds = 30)
        : max_iter(max_iter), max_seconds(max_seconds)
    {
        tree = new Tree(starting_state);
    }
    ~Agent()
    {
        delete tree;
    }
    const Move *genmove(const Move *enemy_move)
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
    const State *get_current_state() const
    {
        return tree->get_current_state();
    }
    void feedback() const { tree->print_stats(); }
};

int main()
{
    State *newstate = new State();
    newstate->print();
    // Move *myMove = new Move(0, 0, 1);
    // newstate = newstate->next_state(myMove);
    // newstate->print();
    Agent agentt(newstate, 10000);
    Move *enemy_movesd = NULL;
    int winner;
    do
    {
        int x, y;
        std::cout << "x : " << std::endl;
        std::cin >> x;
        std::cout << "y : " << std::endl;
        std::cin >> y;
        int player;
        std::cout << "player : " << std::endl;
        std::cin >> player;
        newstate->turn = player;
        Move *myMove = new Move(x, y, player);
        newstate = newstate->next_state(myMove);
        newstate->print();

        agentt.feedback();
        agentt.genmove(myMove); // Note: we ignore the move because the MCTS tree has played it as well and we can get it from there.
        const State *new_state = agentt.get_current_state();
        new_state->print();
        if (new_state->is_terminal())
        {
            winner = ((const State *)new_state)->get_winner();
            break;
        }
    } while (true);
    // agentt.genmove(myMove);

    return 0;
}