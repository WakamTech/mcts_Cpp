#ifndef __STATE_HPP__
#define __STATE_HPP__

#include "Move.hpp"


class State
{
private:
    /* data */
public:
    State();
    State(const State &other);

    int board[3][3]{};

    bool player_won(char player) const;
    int calculate_winner() const;
    int turn, winner;
    void change_turn();

public:
    int get_turn() const;
    int get_winner() const;
    bool is_terminal() const;
    State *next_state(const Move *move) const ;

    std::vector<Move *> *actions_to_try() const;

    double rollout() const;
   
    void print() const;

    bool player1_turn() const ;
    std::vector<int> state_to_model_input();
};

#endif //__STATE_HPP__