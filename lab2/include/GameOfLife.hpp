#ifndef __GAMEOFLIFE_H__
#define __GAMEOFLIFE_H__
#include <SFML/Graphics.hpp>
#include <array>

template <size_t gridWidth, size_t gridHeight>
class GameOfLife
{
    public:
        explicit GameOfLife();
        
    private:
        // int countNeighborsSEQ(const )
    private:
       std::array<std::array<gridWidth, bool>, gridHeight> _grid;

};
#endif // __GAMEOFLIFE_H__