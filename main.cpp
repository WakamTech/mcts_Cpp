#include <iostream>
#include <random>
#include <map>
#include <iomanip>
#include <algorithm>

#include "State.hpp"
#include "Agent.hpp"

std::vector<int> moves;

void print_database(std::vector<int> data_moves)
{
    std::vector<int> validation = {10, 11, -10};
    for (auto elm : data_moves)
    {
        std::cout << elm << " ";
        auto find_end = std::find(validation.begin(), validation.end(), elm) != validation.end();
        if (find_end)
        {
            std::cout << "\n";
        }
    }
}

void print_(std::vector<int> data_moves)
{
    for (auto elm : data_moves)
    {
        std::cout << elm << " ";
    }
}
void simulate_full_game(State *state, std::map<std::vector<int>, std::vector<int>> & states_moves)
{
    auto available_moves = state->actions_to_try();
    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Generate a random index within the range of valid indices
    std::uniform_int_distribution<> dist(0, available_moves->size() - 1);
    int randomIndex = dist(gen);

    // Access the element at the random index
    auto random_move = available_moves->at(randomIndex);
    std::vector<int> board;
    if (state->get_turn() == 1)
    {
        for (int i = 0; i < 9; ++i)
        {
            board.push_back(state->board[i / 3][i % 3]);
        }
        std::vector<int> made_move = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        made_move[3 * random_move->x + random_move->y] = 1;
        states_moves.insert(std::make_pair(board, made_move));
    }
    state = state->next_state(random_move);
    moves.push_back(3*random_move->x + random_move->y);

    state->print();
    if (!state->is_terminal())
    {
        simulate_full_game(state, states_moves);
    }
    else if (state->is_terminal())
    {
        if (state->get_winner() == 1)
            moves.push_back(10);
        else if (state->get_winner() == 2)
            moves.push_back(11);
        else if (state->get_winner() == -1)
            moves.push_back(-10);
        std::cout << "ok done" << std::endl;
    }
}

void games(State *state)
{
    std::map<std::vector<int>, std::vector<int>> states_moves;

    simulate_full_game(state, states_moves);
    state->print();

    for (std::map<std::vector<int>, std::vector<int>>::reverse_iterator it = states_moves.rbegin(); it != states_moves.rend(); ++it)
    {
        print_(it->first);
        std::cout << " -> ";
        print_(it->second);
        std::cout << "\n\n";
    }
    //std::cout << state->print << std::endl;
    state->print();
}
int main()
{
    State *newstate = new State();
    newstate->print();
    Agent agentt(newstate, 1000);
    Move *enemy_movesd = NULL;
    int winner;
    bool done = false;
    /*do
    {
        int x, y;
        std::cout << "x : " << std::endl;
        std::cin >> x;
        std::cout << "y : " << std::endl;
        std::cin >> y;
        int player;
        std::cout << "player : " << std::endl;
        std::cin >> player;

        Move *myMove = new Move(x, y, player);
        newstate = newstate->next_state(myMove);
        newstate->print();

        if (newstate->is_terminal())
        {
            winner = newstate->get_winner();
            done = true;
            break;
        }

        agentt.feedback();
        agentt.genmove(myMove); // Note: we ignore the move because the MCTS tree has played it as well and we can get it from there.
        newstate = (State *)agentt.get_current_state();

        newstate->print();
        if (newstate->is_terminal())
        {
            winner = newstate->get_winner();
            done = true;
            break;
        }
    } while (!done);*/
    State *newsstate = new State();
    /*for (size_t i = 0; i < 2; i++)
    {
        simulate_full_game(1, newsstate);
    }*/
    games(newsstate);
    print_database(moves);
    return 0;
}