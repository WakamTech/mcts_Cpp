#include <iostream>
#include <random>
#include <map>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <cstdio>

#include "State.hpp"
#include "Agent.hpp"
#include "CustomDataset.hpp"
#include "Model.hpp"

#include <iostream>

#define DATASET_FILE_NAME "dataset.csv"
#define TARGET_FILE_NAME "target.csv"

std::vector<int> moves;
State *myState;

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
void simulate_full_game(State *state, std::map<std::vector<int>, std::vector<int>> &states_moves)
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
    moves.push_back(3 * random_move->x + random_move->y);

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
        myState = state;
    }
}

void write_to_csv(std::map<std::vector<int>, std::vector<int>> states_moves)
{
    for (std::map<std::vector<int>, std::vector<int>>::reverse_iterator it = states_moves.rbegin(); it != states_moves.rend(); ++it)
    {
        print_(it->first);
        std::cout << " -> ";
        print_(it->second);
        std::cout << "\n\n";
        std::ofstream dataset(DATASET_FILE_NAME, std::ios::app);
        std::ofstream target(TARGET_FILE_NAME, std::ios::app);
        for (auto e : it->first)
        {
            dataset << e << ",";
        }
        dataset << "\n";
        dataset.close();
        for (auto e : it->second)
        {
            target << e << ",";
        }
        target << "\n";
        target.close();
    }
}
void games(State *state)
{
    std::map<std::vector<int>, std::vector<int>> states_moves;

    simulate_full_game(state, states_moves);
    state = myState;
    myState = NULL;
    if (state->get_winner() == 1 || state->get_winner() == 2)
    {
        write_to_csv(states_moves);
    }
}
std::vector<float> convert_to_vector(std::vector<float> vector, int player)
{
    std::vector<float> board_to_vec;
    if (player == 1)
    {
        for (int i = 0; i < 9; ++i)
            board_to_vec.push_back((float)(vector[i]));
    }
    else
    {
        board_to_vec = vector;

        for (int i = 0; i < 9; i++)
        {
            if (vector[i] != 0)
                board_to_vec[i] = -vector[i];
        }
    }
    return board_to_vec;
}

int main()
{
    auto loaded_net = Net();
    torch::load(loaded_net, "simple_net.pt");

    State *newstate = new State();
    newstate->print();
    Agent agentt(newstate,loaded_net, 1000, 30);
    Move *enemy_movesd = NULL;
    int winner;
    bool done = false;

    auto net = Net();
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
    } while (!done);
    //auto net = Net();

    //State *newsstate = new State();
    /*for (size_t i = 0; i < 60000; i++)
    {
        games(newsstate);
    }*/
    /*print_database(moves);
    std::cout << std::fixed << std::setprecision(6);
    auto dataset = CustomDataset(DATASET_FILE_NAME, TARGET_FILE_NAME)
                       .map(torch::data::transforms::Stack<>());

    auto data_loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
        std::move(dataset),
        2000);

    torch::optim::Adam optimizer(net->parameters(), torch::optim::AdamOptions(0.03));

    auto criterion = torch::nn::BCELoss();
    std::cout << "aa" << std::endl;
    int a;
    std::cin >> a;
    std::cerr << "tarining -> [";
    for (int epoch = 0; epoch < 4000; ++epoch)
    {
        for (auto &batch : *data_loader)
        {
            // std::cout << "Data: " << batch.data << std::endl;
            // std::cout << "Target: " << batch.target << std::endl;
            auto data = batch.data;
            auto labels = batch.target;

            // Forward pass
            auto output = net->forward(data);
            auto loss = criterion(output.to(torch::kFloat), labels.to((torch::kFloat)));

            // Backward pass and optimize                       
            optimizer.zero_grad();
            loss.backward();                                
            optimizer.step();
            //std::cout << " epoch : " << epoch << "; loss : " << loss.item<float>() << std::endl;
            // std::cout << "Epoch: " << epoch << ", Loss: " << loss.item<float>() << std::endl;
        }
        if((epoch+1)%40==0)
            std::cerr << "#";
    }
    std::cerr << "]\n";*/

    std::cout << "Training finished" << std::endl;
    // Save the trained model
    //torch::save(net, "simple_net.pt");
    // Load the saved model
    //auto loaded_net = Net();
    //torch::load(loaded_net, "simple_net.pt");

    std::vector<float> board = {1, 0, 0, 1, 0, 0, 0, -1, -1};
    /*for (int i = 0; i < 9; ++i)
    {
        board.push_back(newsstate->board[i / 3][i % 3]);
    }*/
    // board[4]=1, board[0]=-1;
    /*board = convert_to_vector(board, -1);
    auto test_input = torch::from_blob(board.data(), {1, 9});
    std::cout << test_input << std::endl;
    auto test_output = loaded_net->forward(test_input);
    std::cout << "Test input: \n" << test_input.view({3,3})  << "\n"
              << std::endl;
    // newsstate->print() ;
    std::cout << "Test output: " << test_output<< std::endl;
    std::cout << "Test output chosen position : " << torch::argmax(test_output).item<int>() << std::endl;*/
    /*newsstate = new State();
    Node *parent;
    Node *root = new Node(parent, newsstate, new Move(1, 1, 1), loaded_net);
    root->expand();
    root->select_best_child(0.2);*/
    return 0;
}
