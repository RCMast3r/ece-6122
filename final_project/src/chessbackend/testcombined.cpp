#include <ChessBackend.h>
#include <ChessStateManager.h>

#include <iostream>
#include <string>

// Assuming ECE_ChessEngine and ChessStateManager are predefined elsewhere
int main() {
    ECE_ChessEngine backend;
    ChessStateManager boardState;

    backend.InitializeEngine();
    backend.sendMove("uci");
    backend.sendMove("isready");

    // Variable to store user input and engine response
    std::string input, out;
    std::string moves = "";  // This will hold the list of moves made so far

    // Initial prompt
    std::cout << "Enter your move: ";

    // Infinite loop to keep accepting moves
    while (true) {
        // Read user input (move)
        std::getline(std::cin, input);

        boardState.applyMove(input);
        // Exit the loop if the user types "exit"
        if (input == "exit") {
            break;
        }

        // Append the user's move to the list of moves
        moves += " " + input;
        backend.sendMove(std::string("position startpos moves") + moves);

        // Request engine's best move
        backend.sendMove("go depth 10");

        // Clear the response string
        out = "";

        // Wait for the engine's response
        while (out.find("bestmove") == std::string::npos) {
            (void)backend.getResponseMove(out);
        }

        // Extract the best move from the engine's response
        auto move = out.substr(out.find("bestmove") + 9, 4);
        std::cout << "Engine's best move: " << move << std::endl;

        boardState.applyMove(move);
        // Append the engine's move to the list of moves
        moves += " " + move;

        // Print the updated board state after the engine's move
        boardState.printBoard();

        // Prompt for the next user input
        std::cout << "Enter your move: ";
    }

    std::cout << "Game over!" << std::endl;
    return 0;
}
