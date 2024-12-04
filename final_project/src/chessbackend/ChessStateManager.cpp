#include <ChessStateManager.h>
    
std::pair<int, int> ChessStateManager::_algebraicToIndex(const std::string &pos) {
    if (pos.size() != 2 || pos[0] < 'a' || pos[0] > 'h' || pos[1] < '1' || pos[1] > '8') {
        throw std::invalid_argument("Invalid position: " + pos);
    }
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
std::string ChessStateManager::applyMove(const std::string &move) {
    if (move.size() != 4) {
        throw std::invalid_argument("Move must be in the format 'e2e4'.");
    }
    auto [startRow, startCol] = _algebraicToIndex(move.substr(0, 2));
    auto [endRow, endCol] = _algebraicToIndex(move.substr(2, 2));

    char piece = _board[startRow][startCol];
    if (piece == '.') {
        throw std::invalid_argument("No piece at the starting position.");
    }

    // Make the move
    _board[startRow][startCol] = '.';
    _board[endRow][endCol] = piece;

    // Update active color
    _activeColor = (_activeColor == 'w') ? 'b' : 'w';

    // Increment fullmove number if black just played
    if (_activeColor == 'w') {
        _fullmoveNumber++;
    }

    // Reset halfmove clock for captures or pawn moves
    if (piece == 'P' || piece == 'p' || _board[endRow][endCol] != '.') {
        _halfmoveClock = 0;
    } else {
        _halfmoveClock++;
    }

    // En passant and castling rights are not handled here for simplicity

    return generateFEN();
}
