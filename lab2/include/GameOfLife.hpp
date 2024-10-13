/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 10/12/24
Description: game of life header description. contains some function implementations as well for the simple functions.
*/
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

/// @brief the class that contains all of the implementations of the different ways to update the game of life thread. also contains the grids themselves.
class GameOfLife
{
  public:
    /// @brief constructor for the game of life class
    /// @param threadingModelIndex the index derived from the command line arguments to determine what threading mode to use
    /// @param numThreads the number of threads to spawn if under a threading supported mode
    /// @param width width derived from window width, height and cell size
    /// @param height height derived from window width, height and cell size
    explicit GameOfLife(int threadingModelIndex, std::size_t numThreads,
                        std::size_t width, std::size_t height);

    /// @brief handler that calls the internal methods for updating the grid. calls the one that was specified to use (SEQ/THRD/OMP)
    void updateGrid();

    /// @brief getter for grids
    /// @return pair of references to the grids
    std::pair<std::vector<std::vector<bool>> &,
              std::vector<std::vector<bool>> &>
    getRefs()
    {
        return {_prevGrid, _grid};
    }
    /// @brief handles the intialization of the game of life class. will
    /// conditionally start all of the std::threads
    void init()
    {

        _generateGrid(_grid);
        if (_threadingModelIndex == 1)
        {
            for (int i = 0; (i < _numThreads); i++)
            {
                _threads.push_back(
                    std::thread(&GameOfLife::_handleGridUpdate, this, i));
            }
        }
    }

  private:
    /// @brief handler for single threaded grid updating
    void _updateGridSEQ();
    
    /// @brief the handler for kicking the multi-threaded grid update threads. waits for the threads to finish 
    //         by waiting on a second condition variable kicked by a std::barrier handler function
    void _updateGridThreaded();
    
    /// @brief openmp multi-threading handler for grid updating. 
    /// @param numThreads the number of threads to use
    void _updateGridOpenMPThreaded(std::size_t numThreads);

    /// @brief handler given to the std::barrier for signaling that all the std::threads have finished
    void _onGridPopulation()
    {
        {
            std::unique_lock lk(_retMtx);
            _gridPopulated = true;
        }
        if (_count++ % 2)
        {
            // switching the refs
            _prevGridRef = _grid;
            _gridRef = _prevGrid;
        }
        _retCv.notify_all(); // notify all the other threads that are waiting on this condition variable. should just be the main thread
    }

    /// @brief grid generator for randomizing grid initially
    /// @param grid the grid to initialize
    void _generateGrid(std::vector<std::vector<bool>> &grid);

    /// @brief the grid updating function that gets run in the thread. has a while(true) loop for staying alive and waits on a condition variable to start
    /// @param index the index of this thread. gets used with the for statement to 
    void _handleGridUpdate(int index);

    /// @brief static inlined function for counting the neighbors
    /// @param grid the ref to the grid to check. will be the prevgridref
    /// @param x the width to check 
    /// @param y the height to check
    /// @param width width of the grid itself
    /// @param height height of the grid
    /// @return neighbors that are alive around the desired coordinate
    static inline int
    _countNeighbors(const std::vector<std::vector<bool>> &grid, std::size_t x,
                    std::size_t y, std::size_t width, std::size_t height)
    {
        int count = 0;
        for (int dx = -1; dx <= 1; ++dx)
        {
            size_t nx = (x + dx + width) % width;
            const auto &col = grid[nx]; // get the column once
            for (int dy = -1; dy <= 1; ++dy)
            {
                if (dx == 0 && dy == 0) continue;
                size_t ny = (y + dy + height) % height;

                count += col[ny];
            }
        }
        return count;
    }

  private:
    std::barrier<std::function<void()>> _threadSync; // condition variable that gets setup during construction of the class 
    std::mutex _gridMtx, _retMtx; // mutexs for the std::thread handler
    std::condition_variable _cv, _retCv;
    bool _gridPopulated = false; // start out un-populated and then 
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