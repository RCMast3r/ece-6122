#ifndef __GAMEOFLIFE_H__
#define __GAMEOFLIFE_H__
#include <SFML/Graphics.hpp>

#include <array>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <thread>
#include <barrier>
#include <condition_variable>
#include <iostream>

template <const std::size_t gridWidth, const std::size_t gridHeight>
class GameOfLife {
  public:
    explicit GameOfLife(std::size_t numThreads);
    void updateGrid();

    std::pair<std::array<std::array<bool, gridHeight>, gridWidth> &,
              std::array<std::array<bool, gridHeight>, gridWidth> &>
    getRefs() {
        return {_prevGrid, _grid};
    }
    void init() {

        _generateGrid(_grid);
        _prevGrid = _grid;
        _gridRef = &_grid;
        _prevGridRef = &_prevGrid;
    }
    void updateGridThreaded();

    void updateGridOpenMPThreaded(std::size_t numThreads);
  private:
    void _onGridPopulation() {
        {
            std::unique_lock lk(_retMtx);
            _gridPopulated = true;
        }
        _prevGrid = std::move(_grid);
        _retCv.notify_all();
    }
    void _generateGrid(std::array<std::array<bool, gridHeight>, gridWidth> &grid);
    void _handleGridUpdate(int index);
    int _countNeighbors(
        const std::array<std::array<bool, gridHeight>, gridWidth> &grid,
        std::size_t x, std::size_t y);

  private:
    std::barrier<std::function<void()>> _threadSync;
    std::mutex _gridMtx, _retMtx;
    std::condition_variable _cv, _retCv;
    bool _gridPopulated = false;
    bool _startPopulateGrid = false;
    uint64_t _count = 0;
    std::size_t _numThreads;
    std::vector<std::thread> _threads;
    std::array<std::array<bool, gridHeight>, gridWidth> *_gridRef;
    std::array<std::array<bool, gridHeight>, gridWidth> *_prevGridRef;
    std::array<std::array<bool, gridHeight>, gridWidth> _grid;
    std::array<std::array<bool, gridHeight>, gridWidth> _prevGrid;
    std::function<void(int &)> _countAdd;
};

#include "GameOfLife.tpp"

#endif // __GAMEOFLIFE_H__