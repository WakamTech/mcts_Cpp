#include "State.hpp"

State::State() : turn(1)
{
    // initialize board as empty
    for (int i = 0; i < 9; i++)
    {
        board[i / 3][i % 3] = 0;
    }
    // calculate winner
    winner = calculate_winner();
}
State::State(const State &other) : turn(other.turn), winner(other.winner)
{
    // copy board
    for (int i = 0; i < 9; i++)
    {
        board[i / 3][i % 3] = other.board[i / 3][i % 3];
    }
}

bool State::player_won(char player) const
{
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
int State::calculate_winner() const
{
    if (player_won(1))
        return 1;
    else if (player_won(-1))
        return -1;
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

void State::change_turn() { turn = (turn == 1) ? -1 : 1; }

int State::get_turn() const { return turn; }
int State::get_winner() const { return winner; }
bool State::is_terminal() const
{
    return calculate_winner() != 0;
}

State *State::next_state(const Move *move) const
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

std::vector<Move *> *State::actions_to_try() const
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

double State::rollout() const
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
            std::cerr << "Warning: Ran out of available moves and state is not terminal?" << std::endl;
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
void State::print() const
{
    for (int i = 0; i < 9; i++)
    {
        std::cout << board[i / 3][i % 3] << " ";
        if ((i + 1) % 3 == 0)
            std::cout << "\n";
    }
    std::cout << "\n\n";
}
bool State::player1_turn() const { return turn == '1'; }
