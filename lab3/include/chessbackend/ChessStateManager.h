/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 12/07/24
Description: header file for chess baord state manager. can validate moves and handle manipulation 
 of chess pieces themselves for their desired positions they are moving to
*/

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

#include <chesspiece.hpp>

class ChessStateManager
{
public:
    ChessStateManager(std::vector<ChessPiece>& chessPieces) : _pieces(chessPieces){};

    // Generate FEN from the current board state
    std::string generateFEN();
    void parseFEN(const std::string &fen);
    void printBoard() const;
    bool applyMove(const std::string &move);
    void resetBoard();
    bool validateMove(const std::string &move);
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

    // Attack validation functions
    bool _bishopAttacks(int startRow, int startCol, int endRow, int endCol);
    bool _queenAttacks(int startRow, int startCol, int endRow, int endCol);
    bool _kingAttacks(int startRow, int startCol, int endRow, int endCol);
    bool _rookAttacks(int startRow, int startCol, int endRow, int endCol);
    bool _knightAttacks(int startRow, int startCol, int endRow, int endCol);
    bool _pawnAttacks(int startRow, int startCol, int endRow, int endCol, char pawn);

    // Generalized attack check
    bool _canAttack(int attackerRow, int attackerCol, int targetRow, int targetCol);

    // Check functions
    bool _isKingInCheck(char king);

    // transformation functions for handling transform of positions to relative coords
    std::pair<int, int> _algebraicToChessBoardCoord(const std::string &pos);
    std::pair<int, int> _translateToBoardCoords(std::pair<int, int> row_col);
private:
    // internal chess board
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
    std::vector<ChessPiece>& _pieces;

};
#endif // __CHESSSTATEMANAGER_H__