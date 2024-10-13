#include <GameOfLife.hpp>
#include <algorithm>
#include <iostream>
#include <shared_mutex>
#include <syncstream>
#include <barrier>
#include <thread>
#include <functional>
#include <omp.h>
#include <cmath>

GameOfLife::GameOfLife(int threadingModelIndex, std::size_t numThreads,
                       std::size_t width, std::size_t height)
    : _threadSync(numThreads,
                  std::function<void()>([this]() { _onGridPopulation(); })),
      _grid(width, std::vector<bool>(height, false)),
      _prevGrid(width, std::vector<bool>(height, false)),
      _prevGridRef(_prevGrid), _gridRef(_grid) {
    _numThreads = numThreads;
    // set the number of threads globally for openmp (only matters if running
    // under openmp mode)

    _gridWidth = _grid.size();
    _gridHeight = _grid[0].size();
    _threadingModelIndex = threadingModelIndex;
}

void GameOfLife::_generateGrid(std::vector<std::vector<bool>> &grid) {
    std::cout << " width: " << _gridWidth << std::endl;
    std::cout << " height: " << _gridHeight << std::endl;
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (std::size_t x = 0; x < _gridWidth; ++x) {
        for (std::size_t y = 0; y < _gridHeight; ++y) {
            auto test = std::rand() % 2;

            grid[x][y] = (test == 0); // Randomly seed each pixel
        }
    }
}

void GameOfLife::updateGrid() {
    switch (_threadingModelIndex) {
    case 0: {
        _updateGridSEQ();
        break;
    }
    case 1: {
        _updateGridThreaded();
        break;
    }
    case 2: {
        _updateGridOpenMPThreaded(_numThreads);
        
        break;
    }
    }
}

void GameOfLife::_updateGridSEQ() {
    for (int x = 0; x < _gridWidth; ++x) {
        for (int y = 0; y < _gridHeight; ++y) {
            int neighbors =
                _countNeighbors(_prevGridRef, x, y, _gridWidth, _gridHeight);
            // std::cout << "x " << x << std::endl;
            // std::cout << "y " << y << std::endl;
            if (_prevGridRef[x][y]) {
                if (neighbors < 2 || neighbors > 3) {
                    _gridRef[x][y] = false; // Cell dies
                }
            } else {
                if (neighbors == 3) {
                    _gridRef[x][y] = true; // Cell becomes alive
                }
            }
        }
    }
    if (_count++ % 2) {
        _prevGridRef = _grid;
        _gridRef = _prevGrid;
    }
}

void GameOfLife::_handleGridUpdate(int index) {
    int thisThreadIndex = index;

    auto verticalWidthSize = std::floor(_gridWidth / _numThreads);

    auto startInd = (verticalWidthSize * thisThreadIndex);

    // handle small last little bit that does not get updated. in the last
    // thread we need to increase the width size to get to the rest of the
    // screen
    if (thisThreadIndex == (_numThreads - 1)) {
        verticalWidthSize = _gridWidth - startInd;
    }

    while (true) {
        {
            std::unique_lock lk(_gridMtx);
            _cv.wait(lk, [this] { return _startPopulateGrid; });
        }

        auto prevGridBegin = _prevGridRef.begin();
        auto gridBegin = _gridRef.begin();

        for (int x = 0; x < verticalWidthSize; ++x) {
            std::size_t x_loc = startInd + x;
            for (int y = 0; y < _gridHeight; ++y) {
                int neighbors = _countNeighbors(_prevGridRef, x_loc, y,
                                                _gridWidth, _gridHeight);
                if (_prevGridRef[x_loc][y]) {
                    if (neighbors < 2 || neighbors > 3) {
                        _gridRef[x_loc][y] = false; // Cell dies
                    }
                } else {
                    if (neighbors == 3) {
                        _gridRef[x_loc][y] = true; // Cell becomes alive
                    }
                }
            }
        }
        _threadSync.arrive_and_wait();
    }
}

void GameOfLife::_updateGridThreaded() {
    _gridPopulated = false;

    {
        std::unique_lock lk(_gridMtx);
        _startPopulateGrid = true;
    }
    // std::cout <<"updating" <<std::endl;
    _cv.notify_all();

    {
        std::unique_lock lk(_retMtx);
        _retCv.wait(lk, [this] { return _gridPopulated; });
    }
}

void GameOfLife::_updateGridOpenMPThreaded(std::size_t numThreads) {
    int x;
    #pragma omp parallel for num_threads(numThreads)
       for (x = 0; x < _gridWidth; ++x) {
        for (int y = 0; y < _gridHeight; ++y) {
            int neighbors =
                _countNeighbors(_prevGridRef, x, y, _gridWidth, _gridHeight);
            // std::cout << "x " << x << std::endl;
            // std::cout << "y " << y << std::endl;
            if (_prevGridRef[x][y]) {
                if (neighbors < 2 || neighbors > 3) {
                    _gridRef[x][y] = false; // Cell dies
                }
            } else {
                if (neighbors == 3) {
                    _gridRef[x][y] = true; // Cell becomes alive
                }
            }
        }
    }
    if (_count++ % 2) {
        _prevGridRef = _grid;
        _gridRef = _prevGrid;
    } 
        
}