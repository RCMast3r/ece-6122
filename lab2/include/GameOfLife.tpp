#include <GameOfLife.hpp>
#include <algorithm>
#include <iostream>
#include <shared_mutex>
#include <syncstream>
#include <barrier>
#include <thread>
#include <functional>
#include <omp.h>

template <const size_t gridWidth, const size_t gridHeight>
GameOfLife<gridWidth, gridHeight>::GameOfLife(std::size_t numThreads)
    : _threadSync(
          numThreads, std::function<void()>([this]() { _onGridPopulation(); })) {
    _grid.fill({});
    _prevGrid.fill({});

    _numThreads = numThreads;
    for (int i = 0; i < numThreads; i++) {
        _threads.push_back(std::thread(
            &GameOfLife<gridWidth, gridHeight>::_handleGridUpdate, this, i));
    }
}

template <const std::size_t gridWidth, const std::size_t gridHeight>
void GameOfLife<gridWidth, gridHeight>::_generateGrid(
    std::array<std::array<bool, gridHeight>, gridWidth> &grid) {
    std::cout << "size x: " << grid.size() << " width: " << gridWidth
              << std::endl;
    std::cout << "size y: " << grid[0].size() << " height: " << gridHeight
              << std::endl;
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (std::size_t x = 0; x < gridWidth; ++x) {
        for (std::size_t y = 0; y < gridHeight; ++y) {
            auto test = std::rand() % 2;

            grid[x][y] = (test == 0); // Randomly seed each pixel
        }
    }
}

template <const size_t gridWidth, const size_t gridHeight>
int GameOfLife<gridWidth, gridHeight>::_countNeighbors(
    const std::array<std::array<bool, gridHeight>, gridWidth> &grid,
    std::size_t x, std::size_t y) {
    int count = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) {
                continue;
            }
            size_t nx = (x + dx + gridWidth) % gridWidth;
            size_t ny = (y + dy + gridHeight) % gridHeight;
            count += grid[nx][ny];
        }
    }
    return count;
}

template <const size_t gridWidth, const size_t gridHeight>
void GameOfLife<gridWidth, gridHeight>::updateGrid() {

    for (int x = 0; x < gridWidth; ++x) {
        for (int y = 0; y < gridHeight; ++y) {
            int neighbors = _countNeighbors(_prevGrid, x, y);
            if (_prevGrid[x][y]) {
                if (neighbors < 2 || neighbors > 3) {
                    _grid[x][y] = false; // Cell dies
                }
            } else {
                if (neighbors == 3) {
                    _grid[x][y] = true; // Cell becomes alive
                }
            }
        }
    }
    if(_count++ %2)
    {

        _prevGrid = std::move(_grid);
    }
}

template <const size_t gridWidth, const size_t gridHeight>
void GameOfLife<gridWidth, gridHeight>::_handleGridUpdate(int index) {
    int thisThreadIndex = index;
    
    auto verticalWidthSize = gridWidth / _numThreads;
    // std::cout << verticalWidthSize<<std::endl; 
    while (true) {
        {
            std::unique_lock lk(_gridMtx);
            _cv.wait(lk, [this] { return _startPopulateGrid; });
        }
        for (int x = 0; x < verticalWidthSize; ++x) {
            std::size_t x_loc = (verticalWidthSize * thisThreadIndex) + x;
            for (int y = 0; y < gridHeight; ++y) {
                int neighbors = _countNeighbors(_prevGrid, x_loc, y);
                if (_prevGrid[x_loc][y]) {
                    if (neighbors < 2 || neighbors > 3) {
                        _grid[x_loc][y] = false; // Cell dies
                    }
                } else {
                    if (neighbors == 3) {
                        _grid[x_loc][y] = true; // Cell becomes alive
                    }
                }
            }
        }
        _threadSync.arrive_and_wait();
    }
}

template <const size_t gridWidth, const size_t gridHeight>
void GameOfLife<gridWidth, gridHeight>::updateGridThreaded() {
    _gridPopulated = false;

    {
        std::unique_lock lk(_gridMtx);
        _startPopulateGrid = true;
    }

    _cv.notify_all();

    {
        std::unique_lock lk(_retMtx);
        _retCv.wait(lk, [this] { return _gridPopulated; });
    }
}

template <const size_t gridWidth, const size_t gridHeight>
void GameOfLife<gridWidth, gridHeight>::updateGridOpenMPThreaded(std::size_t numThreads) {
    int x, y;
    omp_set_dynamic(0);
    omp_set_num_threads(numThreads);
    #pragma omp parallel for
    for (x = 0; x < gridWidth; ++x) {
        #pragma omp parallel for
        for (y = 0; y < gridHeight; ++y) {
            int neighbors = _countNeighbors(_prevGrid, x, y);
            if (_prevGrid[x][y]) {
                if (neighbors < 2 || neighbors > 3) {
                    _grid[x][y] = false; // Cell dies
                }
            } else {
                if (neighbors == 3) {
                    _grid[x][y] = true; // Cell becomes alive
                }
            }
        }
    }
    _prevGrid = std::move(_grid);
}
