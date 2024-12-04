#include <ChessStateManager.h>
#include <cstring> 
    
std::pair<int, int> ChessStateManager::_algebraicToIndex(const std::string &pos) {
    
    int col = pos[0] - 'a';
    int row = 8 - (pos[1] - '0');
    return {row, col};
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

// Apply a move and return the updated FEN string
std::optional<std::string> ChessStateManager::applyMove(const std::string &move) {
    if (move.size() != 4) {
        // throw std::invalid_argument("Move must be in the format 'e2e4'.");
        std::cout <<"Invalid command or move!!" <<std::endl;
        return std::nullopt;
    }
    auto pos = move.substr(0, 2);
    if (pos.size() != 2 || pos[0] < 'a' || pos[0] > 'h' || pos[1] < '1' || pos[1] > '8') {
        // throw std::invalid_argument("Invalid position: " + pos);
        std::cout <<"Invalid command or move!!" <<std::endl;
        return std::nullopt;
    }
    pos = move.substr(2, 2);
    if (pos.size() != 2 || pos[0] < 'a' || pos[0] > 'h' || pos[1] < '1' || pos[1] > '8') {
        // throw std::invalid_argument("Invalid position: " + pos);
        std::cout <<"Invalid command or move!!" <<std::endl;
        return std::nullopt;
    }

    auto [startRow, startCol] = _algebraicToIndex(move.substr(0, 2));
    auto [endRow, endCol] = _algebraicToIndex(move.substr(2, 2));

    char piece = _board[startRow][startCol];
    if (piece == '.') {
        // throw std::invalid_argument("No piece at the starting position.");
        std::cout <<"Invalid command or move!!" <<std::endl;
        return std::nullopt;
    }

    bool moveValid = false;

    // Validate the move based on the piece type
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

    if (!moveValid) {
        // throw std::invalid_argument("Illegal move for this piece.");
        std::cout <<"Invalid command or move!!" <<std::endl;
        return std::nullopt;
    }

    if (_isSameColor(endRow, endCol)) {
        std::cout << "Invalid command or move!!" <<std::endl;
        return std::nullopt;
        
    }

    // Apply the move
    _board[endRow][endCol] = piece;
    _board[startRow][startCol] = '.';

    // Switch active color
    _activeColor = (_activeColor == 'w') ? 'b' : 'w';

    return generateFEN();

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