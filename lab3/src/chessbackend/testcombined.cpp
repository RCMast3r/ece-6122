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
    boardState.printBoard();
    std::cout << "Enter your move: ";
    
    // Infinite loop to keep accepting moves
    while (true) {
        // Read user input (move)
        std::getline(std::cin, input);

        if (input == "quit") {
            break;
        }

        auto resp_fen = boardState.applyMove(input);

        if(!resp_fen)
        {
            std::cout << "Enter a real move: ";
            continue;
        }
        boardState.printBoard();
        // Exit the loop if the user types "exit"
        

        // Append the user's move to the list of moves
        moves += " " + input;
        std::cout << moves <<std::endl;
        backend.sendMove(std::string("position startpos moves") + moves);

        // Request engine's best move
        backend.sendMove("go depth 10");

        // Clear the response string
        out = "";

        // Wait for the engine's response
        while ( (out.find("bestmove"))== std::string::npos) {

            (void)backend.getResponseMove(out);
            std::cout << out <<std::endl;
        }
        if(out.find("Cannot execute move") != std::string::npos)
        {
            std::cout << out <<std::endl;
            moves.erase(moves.size() - 5);
            
            std::cout <<"Invalid command or move !!" <<std::endl;
            std::cout << "Enter a real move: ";
            continue;
        }

        // Extract the best move from the engine's response
        auto move = out.substr(out.find("bestmove") + 9, 4);
        std::cout << "Engine's best move: " << move << std::endl;

        resp_fen = boardState.applyMove(move);
        if(!resp_fen)
        {
            std::cout << "engine gave bad move, exiting. please restart program." << std::endl;
            return 0;
        }

        if(out.find("ponder 0000") !=std::string::npos)
        {
            boardState.printBoard();
            std::cout << "Checkmate you lose!!" << std::endl;
            boardState.resetBoard();
            moves = "";
            std::cout << "Enter your move: "; // print before going back to start of game loop
            
            continue;
        }
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
