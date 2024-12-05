#ifndef __CHESSSTATEMANAGER_H__
#define __CHESSSTATEMANAGER_H__
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <utility>
#include <optional>
class ChessStateManager
{
public:
    ChessStateManager() {};


    // Generate FEN from the current board state
    std::string generateFEN();
    void parseFEN(const std::string &fen);
    void printBoard() const;
    std::optional<std::string> applyMove(const std::string &move);
    void resetBoard();
private:
    /// @brief Convert algebraic notation (e.g., "e2") to board indices
    /// @param pos the string to get position from index-wise
    /// @return the coords
    std::pair<int, int> _algebraicToIndex(const std::string &pos);
    bool _isSameColor(int row, int col);
    bool _validatePawnMove(int startRow, int startCol, int endRow, int endCol);
    bool _validateRookMove(int startRow, int startCol, int endRow, int endCol);
    bool _validateKnightMove(int startRow, int startCol, int endRow, int endCol);
    bool _validateBishopMove(int startRow, int startCol, int endRow, int endCol);
    bool _validateQueenMove(int startRow, int startCol, int endRow, int endCol);
    bool _validateKingMove(int startRow, int startCol, int endRow, int endCol);
    
private:
    char _board[8][8] = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };
    

    char _activeColor = 'w';
    std::string _castlingRights = "KQkq";
    std::string _enPassant = "-";
    int _halfmoveClock = 0;
    int _fullmoveNumber = 1;

};
#endif // __CHESSSTATEMANAGER_H__