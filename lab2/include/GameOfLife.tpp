#include <GameOfLife.hpp>
#include <algorithm>
#include <iostream>

template <const size_t gridWidth, const size_t gridHeight>
GameOfLife<gridWidth, gridHeight>::GameOfLife()
{
    _grid = {false};
    _prevGrid = {false};
    
    // _countAdd = [](int& count) { count++; };
}

template <const std::size_t gridWidth, const std::size_t gridHeight>
void GameOfLife<gridWidth, gridHeight>::_generateGrid(std::array<std::array<bool, gridWidth>, gridHeight> &grid)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (std::size_t x = 0; x < gridWidth; ++x)
    {
        for (std::size_t y = 0; y < gridHeight; ++y)
        {
            auto test = std::rand() % 2;
            grid[x][y] = (test == 0); // Randomly seed each pixel
        }
    }
}



template <const size_t gridWidth, const size_t gridHeight>
template <int... I, int... J>
int GameOfLife<gridWidth, gridHeight>::_countNeighbors(const std::array<std::array<bool, gridWidth>, gridHeight> &grid,
                                                       std::size_t gridXLoc,
                                                       std::size_t gridYLoc,
                                                       std::integer_sequence<int, I...>,
                                                       std::integer_sequence<int, J...>)
{
    return ((grid[(gridXLoc - 1 + I + gridWidth) % gridWidth][(gridYLoc - 1 + J + gridHeight) % gridHeight]) + ...);
}

template <const size_t gridWidth, const size_t gridHeight>
std::array<std::array<bool, gridWidth>, gridHeight> &GameOfLife<gridWidth, gridHeight>::updateGrid()
{
    if(_count++ % 2)
    {
        _prevGridRef = _grid;
        _gridRef = _prevGrid;
    } else {
        _prevGridRef = _prevGrid;
        _gridRef = _grid; 
    }
    
    for (int x = 0; x < gridWidth; ++x)
    {
        for (int y = 0; y < gridHeight; ++y)
        {
            auto w = std::make_integer_sequence<int, 3>();
            auto h = std::make_integer_sequence<int, 3>();
            int neighbors = _countNeighbors(_prevGridRef, x, y, std::make_integer_sequence<int, 3>(), std::make_integer_sequence<int, 3>());
            std::cout << neighbors <<std::endl;
            if (_prevGridRef[x][y])
            {
                if (neighbors < 2 || neighbors > 3)
                {
                    _gridRef[x][y] = false; // Cell dies
                }
            }
            else
            {
                if (neighbors == 3)
                {
                    _gridRef[x][y] = true; // Cell becomes alive
                }
            }
        }
    }
    
    return _grid;
}