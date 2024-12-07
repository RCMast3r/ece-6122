#ifndef __CHESSPIECE_H__
#define __CHESSPIECE_H__
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
class ChessPiece
{
    public:
        
        /// @brief board position: bottom right is 0,0, top left is 7,7
        struct BoardPos {
            float x;
            float y;
        };
        enum class ChessPieceType {
            PAWN =0,
            BISHOP,
            KNIGHT,
            ROOK,
            QUEEN,
            KING
        };

        enum class Color {
            WHITE =0,
            BLACK
        };

        struct RenderInfo {
            GLuint texture;
            GLuint vertexBuffer;
            GLuint uvbuffer;
            GLuint normalbuffer;
            GLuint elementbuffer;
            std::size_t indicesSize;
        };

        ChessPiece(ChessPieceType type, Color color, RenderInfo rendInfo, BoardPos pos){
            _type = type;
            _color = color;
            _renderInfo = rendInfo;
            _position = pos;
        }
        // will be set by the chess piece manager class.
        void setPosition(BoardPos pos)
        {
            _position = pos;
        }
        BoardPos getPosition()
        {
            return _position;
        }
        RenderInfo getRenderInfo() { return _renderInfo; }
        
    private:
        ChessPieceType _type;
        RenderInfo _renderInfo;
        BoardPos _position;
        Color _color;
    
};

#endif // __CHESSPIECE_H__