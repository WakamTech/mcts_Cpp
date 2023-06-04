# mcts_C++
# Tic-Tac-Toe

This is a simple implementation of the Tic-Tac-Toe game in C++. The game allows two players to take turns marking spaces on a 3x3 grid, with the goal of getting three of their marks in a row, column, or diagonal.

## How to Play

1. Clone the repository to your local machine.
2. Compile the source code using a C++ compiler.
3. Run the compiled executable.
4. Players will take turns entering their moves by specifying the row and column numbers.
5. The game will continue until a player wins or the board is full.

## Code Structure

- `main.cpp`: The main entry point of the program. It contains the game loop and user input handling.
- `State.h` and `State.cpp`: Defines the `State` class, which represents the game state and provides methods for checking the winner, generating legal moves, and creating new states.
- `Move.h`: Defines the `Move` class, which represents a player's move (coordinates and player number).
- Other necessary header files are included as well well.

## License

This project is licensed under the [MIT License](LICENSE).

Feel free to use and modify the code as per your needs.

## Acknowledgements

This project was inspired by the classic game of Tic-Tac-Toe and is a basic implementation for learning purposes.

## References 
[Cpp-mcts](https://github.com/Konijnendijk/cpp-mcts.git) *https://github.com/Konijnendijk/cpp-mcts.git
