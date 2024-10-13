#ifndef __GAMEOFLIFE_H__
#define __GAMEOFLIFE_H__
#include <SFML/Graphics.hpp>

#include <vector>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <thread>
#include <barrier>
#include <condition_variable>
#include <omp.h>
class GameOfLife {
  public:
    explicit GameOfLife(int threadingModelIndex, std::size_t numThreads,
                        std::size_t width, std::size_t height);
    void updateGrid();

    std::pair<std::vector<std::vector<bool>> &,
              std::vector<std::vector<bool>> &>
    getRefs() {
        return {_prevGrid, _grid};
    }
    void init() {

        _generateGrid(_grid);
        if (_threadingModelIndex == 1) {
            for (int i = 0; (i < _numThreads); i++) {
                _threads.push_back(
                    std::thread(&GameOfLife::_handleGridUpdate, this, i));
            }
        } else if (_threadingModelIndex == 2) {
            // omp_set_dynamic(0);
            // omp_set_num_threads(_numThreads);
        }
    }

  private:
    void _updateGridSEQ();
    void _updateGridThreaded();

    void _updateGridOpenMPThreaded(std::size_t numThreads);

    void _onGridPopulation() {
        {
            std::unique_lock lk(_retMtx);
            _gridPopulated = true;
        }
        if (_count++ % 2) {
            _prevGridRef = _grid;
            _gridRef = _prevGrid;
        }
        _retCv.notify_all();
    }
    void _generateGrid(std::vector<std::vector<bool>> &grid);
    void _handleGridUpdate(int index);
    static inline int
    _countNeighbors(const std::vector<std::vector<bool>> &grid, std::size_t x,
                    std::size_t y, std::size_t width, std::size_t height) {
        int count = 0;
        for (int dx = -1; dx <= 1; ++dx) {
            size_t nx = (x + dx + width) % width;
            const auto &col = grid[nx]; // get the column once
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                size_t ny = (y + dy + height) % height;

                count += col[ny];
            }
        }
        return count;
    }
  private:
    std::barrier<std::function<void()>> _threadSync;
    std::mutex _gridMtx, _retMtx;
    std::condition_variable _cv, _retCv;
    bool _gridPopulated = false;
    bool _startPopulateGrid = false;
    uint64_t _count = 0;
    std::size_t _numThreads, _gridWidth, _gridHeight;
    std::vector<std::thread> _threads;
    std::vector<std::vector<bool>> &_gridRef;
    std::vector<std::vector<bool>> &_prevGridRef;
    std::vector<std::vector<bool>> _grid;
    std::vector<std::vector<bool>> _prevGrid;
    std::function<void(int &)> _countAdd;
    int _threadingModelIndex;
};

#endif // __GAMEOFLIFE_H__