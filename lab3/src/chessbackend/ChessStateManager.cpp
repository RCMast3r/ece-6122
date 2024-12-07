/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 12/07/24
Description: state manager implementation
*/
#include <ChessStateManager.h>
#include <cstring> 
    
std::pair<int, int> ChessStateManager::_algebraicToIndex(const std::string &pos) {
    
    int col = pos[0] - 'a';
    int row = 8 - (pos[1] - '0');
    return {row, col};
}
std::pair<int, int> ChessStateManager::_algebraicToChessBoardCoord(const std::string &pos) {
    
    
    int x = 7- (pos[0] - 'a');           // Convert column 'a'-'h' to 0-7
    int y = (pos[1] - '1');     // Convert row '1'-'8' to 0-7
    return {x, y};
}
std::string ChessStateManager::generateFEN() {
    std::stringstream fen;
    for (int row = 0; row < 8; ++row) {
        int emptyCount = 0;
        for (int col = 0; col < 8; ++col) {
            if (_board[row][col] == '.') {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    fen << emptyCount;
                    emptyCount = 0;
                }
                fen << _board[row][col];
            }
        }
        if (emptyCount > 0) fen << emptyCount;
        if (row != 7) fen << '/';
    }
    fen << ' ' << _activeColor << ' ' << _castlingRights << ' ' << _enPassant
        << ' ' << _halfmoveClock << ' ' << _fullmoveNumber;
    return fen.str();
}

    // Parse FEN string and populate the board
void ChessStateManager::parseFEN(const std::string &fen) {
    std::istringstream iss(fen);
    std::string boardPart, activePart, castlingPart, enPassantPart;
    int halfmove, fullmove;

    iss >> boardPart >> activePart >> castlingPart >> enPassantPart >> halfmove >> fullmove;
    _activeColor = activePart[0];
    _castlingRights = castlingPart;
    _enPassant = enPassantPart;
    _halfmoveClock = halfmove;
    _fullmoveNumber = fullmove;

    // Parse board
    int row = 0, col = 0;
    for (char c : boardPart) {
        if (c == '/') {
            row++;
            col = 0;
        } else if (std::isdigit(c)) {
            int emptyCount = c - '0';
            for (int i = 0; i < emptyCount; ++i) {
                _board[row][col++] = '.';
            }
        } else {
            _board[row][col++] = c;
        }
    }
}

// Print the board
void ChessStateManager::printBoard() const {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            std::cout << _board[row][col] << ' ';
        }
        std::cout << '\n';
    }
}

bool ChessStateManager::validateMove(const std::string &move)
{
    if (move.size() != 4) {
        // throw std::invalid_argument("Move must be in the format 'e2e4'.");
        std::cout <<"Invalid command or move!!" <<std::endl;
        return false;
    }
    auto pos = move.substr(0, 2);
    if (pos.size() != 2 || pos[0] < 'a' || pos[0] > 'h' || pos[1] < '1' || pos[1] > '8') {
        // throw std::invalid_argument("Invalid position: " + pos);
        std::cout <<"Invalid command or move!!" <<std::endl;
        return false;
    }
    pos = move.substr(2, 2);
    if (pos.size() != 2 || pos[0] < 'a' || pos[0] > 'h' || pos[1] < '1' || pos[1] > '8') {
        // throw std::invalid_argument("Invalid position: " + pos);
        std::cout <<"Invalid command or move!! invalid position" <<std::endl;
        return false;
    }

    auto [startRow, startCol] = _algebraicToIndex(move.substr(0, 2));
    auto [endRow, endCol] = _algebraicToIndex(move.substr(2, 2));

    char piece = _board[startRow][startCol];
    char endPiece = _board[endRow][endCol];

    if (piece == '.') {
        // throw std::invalid_argument("No piece at the starting position.");
        std::cout <<"Invalid command or move!! no piece at starting pos" <<std::endl;
        return false;
    }

    bool moveValid = false;

    // detecting castling
    bool castling = false;
    if( (move == "e8g8" && (piece =='k' && _board[0][7] =='r')) || 
     (move == "e1g1" && (piece =='K' && _board[7][7] =='R')))
    {
        moveValid = true;
        castling = true;
    } else {
        if (piece == 'P' || piece == 'p') {
            moveValid = _validatePawnMove(startRow, startCol, endRow, endCol);
        } else if (piece == 'R' || piece == 'r') {
            moveValid = _validateRookMove(startRow, startCol, endRow, endCol);
        } else if (piece == 'N' || piece == 'n') {
            moveValid = _validateKnightMove(startRow, startCol, endRow, endCol);
        } else if (piece == 'B' || piece == 'b') {
            moveValid = _validateBishopMove(startRow, startCol, endRow, endCol);
        } else if (piece == 'Q' || piece == 'q') {
            moveValid = _validateQueenMove(startRow, startCol, endRow, endCol);
        } else if (piece == 'K' || piece == 'k') {
            moveValid = _validateKingMove(startRow, startCol, endRow, endCol);
        }
    }
    // Validate the move based on the piece type
    

    if (!moveValid) {
        // throw std::invalid_argument("Illegal move for this piece.");
        std::cout <<"Invalid command or move!! some type of invalid move" <<std::endl;
        return false;
    }

    if(!castling)
    {
        if (_isSameColor(endRow, endCol)) {
            std::cout << "Invalid command or move!! same color" <<std::endl;
            return false;
        }
    }
    // std::cout << moveValid <<std::endl;/

    // Simulate move
    char capturedPiece = _board[endRow][endCol];
    _board[endRow][endCol] = piece;
    _board[startRow][startCol] = '.';

    // Check king safety
    char king = (std::isupper(piece)) ? 'K' : 'k'; // Determine king by piece color
    bool kingInCheck = _isKingInCheck(king);

    // Revert move
    _board[startRow][startCol] = piece;
    _board[endRow][endCol] = capturedPiece;

    if (kingInCheck) {
        std::cout << "Invalid move! King would be in check." << std::endl;
        return false;
    }

    return moveValid;
}

bool ChessStateManager::applyMove(const std::string &move) {

    auto [startRow, startCol] = _algebraicToIndex(move.substr(0, 2));
    auto [endRow, endCol] = _algebraicToIndex(move.substr(2, 2));

    char piece = _board[startRow][startCol];
    char endPiece = _board[endRow][endCol];
    
    
    auto beginningPos = _algebraicToChessBoardCoord(move.substr(0, 2)); 
    auto targetPos = _algebraicToChessBoardCoord(move.substr(2, 2));

    bool foundStartPiece = false;
    auto start = move.substr(0, 2);
    auto end = move.substr(2, 2);
    // go through all chess pieces and check to ensure that there exists a chess piece at the starting position

    bool castling = (move=="e8g8") || (move =="e1g1");
    for(auto & chessPiece : _pieces)
    {
        // first, we need to set the ending piece
        
        // if there is a chess piece at the ending position, set it's visibility to false.
        if(chessPiece.getUCIPos() == end)
        {
            if(!castling)
            {
                chessPiece.setVisible(false); 
            } else {
                if(move=="e8g8")
                {
                    chessPiece.setGoalPosition({(float)5, (float)7});

                } else {
                    chessPiece.setGoalPosition({(float)5, 0});
                }
            }
        }

        if(chessPiece.getUCIPos() == start)
        {
            chessPiece.setUCIPos(end);
            // Apply the move
            _board[endRow][endCol] = piece;
            _board[startRow][startCol] = '.';
            // std::cout <<"start pos " << chessPiece.getCurrentPosition().x <<" " << chessPiece.getCurrentPosition().y<<std::endl;
            // std::cout <<"end pos " << targetPos.first <<" " << targetPos.second <<std::endl;
            printBoard();
            chessPiece.setGoalPosition({(float)targetPos.first, (float)targetPos.second});
            foundStartPiece = true;
        }
    }

    if(!foundStartPiece)
    {
        std::cout << "Invalid command or move!! no start piece found" <<std::endl;
        return false;
    }
    // Switch active color
    _activeColor = (_activeColor == 'w') ? 'b' : 'w';

    return true;

}

std::pair<int, int> ChessStateManager::_translateToBoardCoords(std::pair<int, int> row_col)
{
   return {row_col.first, 7-row_col.second}; 
}
void ChessStateManager::resetBoard()
{
    const char temp [64] = {
        'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'
        ,'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'
        ,'.', '.', '.', '.', '.', '.', '.', '.'
        ,'.', '.', '.', '.', '.', '.', '.', '.'
        ,'.', '.', '.', '.', '.', '.', '.', '.'
        ,'.', '.', '.', '.', '.', '.', '.', '.'
        ,'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'
        ,'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'};
    ::memcpy(_board, temp, 64);   
}
bool ChessStateManager::_isSameColor(int row, int col) {
        char piece = _board[row][col];
        return (_activeColor == 'w' && std::isupper(piece)) || (_activeColor == 'b' && std::islower(piece));
    }

// Validate pawn move
bool ChessStateManager::_validatePawnMove(int startRow, int startCol, int endRow, int endCol) {
    char pawn = _board[startRow][startCol];
    int direction = (pawn == 'P') ? -1 : 1; // White moves up, black moves down

    // Single square move
    if (endCol == startCol && endRow == startRow + direction && _board[endRow][endCol] == '.') {
        return true;
    }
    // Double square move from the starting position
    if ((pawn == 'P' && startRow == 6 || pawn == 'p' && startRow == 1) &&
        endCol == startCol && endRow == startRow + 2 * direction && _board[endRow][endCol] == '.') {
        return true;
    }
    // Capture move
    if (std::abs(endCol - startCol) == 1 && endRow == startRow + direction && _board[endRow][endCol] != '.' &&
        _isSameColor(endRow, endCol) == false) {
        return true;
    }
    return false;
}

// Validate rook move
bool ChessStateManager::_validateRookMove(int startRow, int startCol, int endRow, int endCol) {
    if (startRow != endRow && startCol != endCol) return false; // Rook moves in straight lines
    if (startRow == endRow) {
        // Horizontal move, check if path is clear
        for (int col = std::min(startCol, endCol) + 1; col < std::max(startCol, endCol); ++col) {
            if (_board[startRow][col] != '.') return false;
        }
    } else if (startCol == endCol) {
        // Vertical move, check if path is clear
        for (int row = std::min(startRow, endRow) + 1; row < std::max(startRow, endRow); ++row) {
            if (_board[row][startCol] != '.') return false;
        }
    }
    return true;
}

// Validate knight move
bool ChessStateManager::_validateKnightMove(int startRow, int startCol, int endRow, int endCol) {
    int rowDiff = std::abs(startRow - endRow);
    int colDiff = std::abs(startCol - endCol);
    return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
}

// Validate bishop move
bool ChessStateManager::_validateBishopMove(int startRow, int startCol, int endRow, int endCol) {
    if (std::abs(startRow - endRow) != std::abs(startCol - endCol)) return false; // Diagonal move
    // Check if path is clear
    int rowStep = (endRow > startRow) ? 1 : -1;
    int colStep = (endCol > startCol) ? 1 : -1;
    int row = startRow + rowStep, col = startCol + colStep;
    while (row != endRow && col != endCol) {
        if (_board[row][col] != '.') return false;
        row += rowStep;
        col += colStep;
    }
    return true;
}

// Validate queen move (combines rook and bishop)
bool ChessStateManager::_validateQueenMove(int startRow, int startCol, int endRow, int endCol) {
    return _validateRookMove(startRow, startCol, endRow, endCol) || _validateBishopMove(startRow, startCol, endRow, endCol);
}

// Validate king move
bool ChessStateManager::_validateKingMove(int startRow, int startCol, int endRow, int endCol) {
    int rowDiff = std::abs(startRow - endRow);
    int colDiff = std::abs(startCol - endCol);
    return rowDiff <= 1 && colDiff <= 1;
}


bool ChessStateManager::_bishopAttacks(int startRow, int startCol, int endRow, int endCol) {
    // Bishop can attack diagonally; check only the path
    if (std::abs(startRow - endRow) != std::abs(startCol - endCol)) return false;
    // Path must be clear
    int rowStep = (endRow > startRow) ? 1 : -1;
    int colStep = (endCol > startCol) ? 1 : -1;
    int row = startRow + rowStep, col = startCol + colStep;
    while (row != endRow && col != endCol) {
        if (_board[row][col] != '.') return false;
        row += rowStep;
        col += colStep;
    }
    return true;
}

bool ChessStateManager::_queenAttacks(int startRow, int startCol, int endRow, int endCol) {
    return _bishopAttacks(startRow, startCol, endRow, endCol) || _rookAttacks(startRow, startCol, endRow, endCol);
}

bool ChessStateManager::_kingAttacks(int startRow, int startCol, int endRow, int endCol) {
    int rowDiff = std::abs(startRow - endRow);
    int colDiff = std::abs(startCol - endCol);
    return rowDiff <= 1 && colDiff <= 1;
}

bool ChessStateManager::_pawnAttacks(int startRow, int startCol, int endRow, int endCol, char pawn) {
    int direction = (pawn == 'P') ? -1 : 1; // White moves up (-1), Black moves down (+1)
    return (endRow - startRow == direction) && (std::abs(endCol - startCol) == 1);
}
bool ChessStateManager::_rookAttacks(int startRow, int startCol, int endRow, int endCol) {
    // Rook attacks in straight lines; check if path is clear
    if (startRow != endRow && startCol != endCol) return false;
    int rowStep = (startRow == endRow) ? 0 : (endRow > startRow ? 1 : -1);
    int colStep = (startCol == endCol) ? 0 : (endCol > startCol ? 1 : -1);
    int row = startRow + rowStep, col = startCol + colStep;
    while (row != endRow || col != endCol) {
        if (_board[row][col] != '.') return false;
        row += rowStep;
        col += colStep;
    }
    return true;
}

bool ChessStateManager::_knightAttacks(int startRow, int startCol, int endRow, int endCol) {
    // Define all possible knight move offsets
    const int knightMoves[8][2] = {
        {2, 1}, {1, 2}, {-1, 2}, {-2, 1},
        {-2, -1}, {-1, -2}, {1, -2}, {2, -1}
    };

    // Check if the target square matches any of the knight's valid moves
    for (const auto& move : knightMoves) {
        int newRow = startRow + move[0];
        int newCol = startCol + move[1];
        if (newRow == endRow && newCol == endCol) {
            return true; // Knight can attack this square
        }
    }

    return false; // No valid knight moves to the target square
}

bool ChessStateManager::_canAttack(int attackerRow, int attackerCol, int targetRow, int targetCol) {
    char attacker = _board[attackerRow][attackerCol];
    if (attacker == '.') return false;

    switch (std::tolower(attacker)) {
        case 'p': return _pawnAttacks(attackerRow, attackerCol, targetRow, targetCol, attacker);
        case 'r': return _rookAttacks(attackerRow, attackerCol, targetRow, targetCol);
        case 'n': return _knightAttacks(attackerRow, attackerCol, targetRow, targetCol);
        case 'b': return _bishopAttacks(attackerRow, attackerCol, targetRow, targetCol);
        case 'q': return _queenAttacks(attackerRow, attackerCol, targetRow, targetCol);
        case 'k': return _kingAttacks(attackerRow, attackerCol, targetRow, targetCol);
        default: return false;
    }
}

bool ChessStateManager::_isKingInCheck(char king) {
    int kingRow = -1, kingCol = -1;

    // Locate the king
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (_board[i][j] == king) {
                kingRow = i;
                kingCol = j;
                break;
            }
        }
        if (kingRow != -1) break;
    }

    if (kingRow == -1 || kingCol == -1) throw std::runtime_error("King not found!");

    // Check if any piece attacks the king
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (_board[i][j] == '.' || _isSameColor(i, j)) continue;
            if (_canAttack(i, j, kingRow, kingCol)) return true;
        }
    }

    return false;
}