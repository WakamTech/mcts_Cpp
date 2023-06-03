#include <iostream>

#include "State.hpp"
#include "Agent.hpp"
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