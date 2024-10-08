#ifndef __GAMEOFLIFE_H__
#define __GAMEOFLIFE_H__
#include <SFML/Graphics.hpp>

#include <cstdlib>
#include <ctime>
#include <array>
#include <cstddef>
#include <functional>
template <const std::size_t gridWidth, const std::size_t gridHeight>
class GameOfLife
{
public:
    explicit GameOfLife();
    /// @brief checks the previous grid (_prevGridRef) and updates _gridRef. then sets the _prevGridRef to _gridRef.
    /// @return _gridRef with updated values
    std::array<std::array<bool, gridWidth>, gridHeight>& updateGrid();
    void init()
    {
        _generateGrid(_prevGrid);
    }
private:
    void _generateGrid(std::array<std::array<bool, gridWidth>, gridHeight> &grid);
    
    template <int ...I, int ...J>
    int _countNeighbors(const std::array<std::array<bool, gridWidth>, gridHeight> &grid, std::size_t gridXLoc, std::size_t gridYLoc, std::integer_sequence<int, I...>, std::integer_sequence<int, J...>);

    
private:
    uint64_t _count = 0;
    std::array<std::array<bool, gridWidth>, gridHeight>& _gridRef, _prevGridRef;
    std::array<std::array<bool, gridWidth>, gridHeight> _grid, _prevGrid;
    std::function<void(int&)> _countAdd;
};

#include "GameOfLife.tpp"

#endif // __GAMEOFLIFE_H__