/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 12/07/24
Description: a test application
*/
#include <ChessStateManager.h>
int main() {
    ChessStateManager chessBoard;

    std::cout << "Initial board:\n";
    chessBoard.printBoard();

    std::string fen = chessBoard.generateFEN();
    std::cout << "\nGenerated FEN: " << fen << "\n";

    try {
        std::cout << "\nApplying move e2e4...\n";
        std::string newFEN = chessBoard.applyMove("e2e4");
        chessBoard.printBoard();
        std::cout << "\nUpdated FEN: " << newFEN << "\n";

        std::cout << "\nApplying move e7e5...\n";
        newFEN = chessBoard.applyMove("e7e5");
        chessBoard.printBoard();
        std::cout << "\nUpdated FEN: " << newFEN << "\n";
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}