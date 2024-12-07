#ifndef __CHESSPIECE_H__
#define __CHESSPIECE_H__
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>

#include <string>
#include <iostream>
class ChessPiece
{
public:
    // Enum for ChessPieceType and Color
    enum class ChessPieceType {
        PAWN = 0,
        BISHOP,
        KNIGHT,
        ROOK,
        QUEEN,
        KING
    };

    enum class Color {
        WHITE = 0,
        BLACK
    };

    // Struct for Board Position (x, y)
    struct BoardPos {
        float x;
        float y;
    };

    // RenderInfo struct for handling OpenGL data
    struct RenderInfo {
        GLuint texture;
        GLuint vertexBuffer;
        GLuint uvbuffer;
        GLuint normalbuffer;
        GLuint elementbuffer;
        std::size_t indicesSize;
    };

    // ChessPiece constructor
    ChessPiece(ChessPieceType type, Color color, RenderInfo rendInfo, BoardPos pos, std::string uciPos)
        : _type(type), _color(color), _renderInfo(rendInfo), _position(pos), _goalPosition(pos), _state(State::STATIONARY), _uciPos(uciPos), _visible(true)
    {
    }

    // Set the goal position and change state to moving
    void setGoalPosition(BoardPos pos)
    {
        _goalPosition = pos;
        _state = State::MOVING;
    }

    void setUCIPos(std::string pos) { _uciPos = pos; }

    std::string getUCIPos() { return _uciPos; }
    void setVisible(bool vis) {_visible = vis;}
    bool isVisible() {return _visible; }
    bool isMoving() {return _state == State::MOVING; }
    bool isKnight() {return _type == ChessPieceType::KNIGHT; }
    Color getColor() {return _color; }

    // Get current position, move towards goal if moving
    BoardPos getCurrentPosition()
    {
        if (_state == State::MOVING)
        {
            // Move towards the goal position, adjust the position gradually
            float speed = 0.01f;  // Control speed of movement (adjust as needed)
            float dx = _goalPosition.x - _position.x;
            float dy = _goalPosition.y - _position.y;
            
            // Move a fraction towards the goal
            _position.x += dx * speed;
            _position.y += dy * speed;

            // std::cout << _position.x <<std::endl; 
            // If the chess piece is close enough to the goal, set state to stationary
            if (std::abs(dx) < 0.01f && std::abs(dy) < 0.01f)
            {
                _position = _goalPosition;  // Snap to goal position when reached
                _state = State::STATIONARY;
            }
        }
        return _position;
    }

    // Get RenderInfo
    RenderInfo getRenderInfo() { return _renderInfo; }

private:
    // Chess piece attributes
    Color _color;
    ChessPieceType _type;
    RenderInfo _renderInfo;
    BoardPos _position;
    BoardPos _goalPosition;
    std::string _uciPos;
    bool _visible;
    // State tracking
    enum class State {
        MOVING,
        STATIONARY
    };

    State _state;  // Current state of the piece (moving or stationary)
};

#endif // __CHESSPIECE_H__