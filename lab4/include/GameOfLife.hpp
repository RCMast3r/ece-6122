/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 11/08/24
Description: header file for the GoL class that also contains the docs for the functions.
*/

#ifndef __GAMEOFLIFE_H__
#define __GAMEOFLIFE_H__
#include <cstdlib>
#include <ctime>
#include <GameOfLifeImplementations.hpp>
#include <cuda.h>
#include <cuda_runtime.h>
#include <iostream>
#include <stdio.h>
#include <assert.h>
// so first i will implement with bool and then after that I will implement using a 
// 2d array of bytes with bit masking operations within each byte to use just one bit per each
// instead of multiple
class GoL
{
    public:
        /// @brief constructor
        /// @param width width of grid
        /// @param height height of grid
        /// @param n number of threads per block
        /// @param cudaMode mode (normal: 0, pinned: 1, managed: 2)
        GoL(std::size_t width, std::size_t height, std::size_t n, int cudaMode); 

        ~GoL(); 
    public:
        bool * getGrid();
        
    private:
        /// @brief test function for evaluating logic
        /// @param current_state current grid
        /// @param prev_state prev grid
        /// @param width width of grid
        /// @param height height of grid
        void _updateGridTest(bool *current_state, bool *prev_state, std::size_t width, std::size_t height);

        /// @brief test function for evaluating logic to count the neighbors. pls ignore
        /// @param state 
        /// @param x 
        /// @param y 
        /// @param width 
        /// @param height 
        /// @return 
        int _countNeighborsTest(bool *state, int x, int y, std::size_t width, std::size_t height);

        /// @brief grid generation function for initialization
        /// @param width width of grid
        /// @param height 
        /// @param grid the pointer to the grid that will be initialized
        /// @return true if success
        bool _generateGrid(std::size_t width, std::size_t height, bool *grid); 

        /// @brief deallocation function for any regular cuda device allocated memory
        /// @param dev_ptr the pointer to free
        void _deallocNormal(bool * dev_ptr);
        /// @brief de-allocator for cuda pinned allocated host memory
        /// @param host_ptr pointer to memory on the host
        void _deallocPinned(bool *host_ptr);

        /// @brief normal cuda allocation function for the grid
        /// @param dev_ptr pointer to the pointer to allocate
        /// @param width width of the grid to allocate
        /// @param height height of grid
        void _allocateNormal(bool **dev_ptr, std::size_t width, std::size_t height);
        
        /// @param width width of the grid to allocate
        /// @param height height of grid
        
        /// @brief cuda allocation function for the grid on host and on-device memory
        /// @param host_ptr pointer to the pointer to the memory to allocate on the host with cuda
        /// @param dev_ptr pointer to the pointer to the memory allocate on the device for one of the 2 grids
        /// @param other_dev_ptr second one of the grids to allocate
        /// @param width width of grid
        /// @param height height of grid
        void _allocatePinned(bool **host_ptr, bool **dev_ptr, bool **other_dev_ptr, std::size_t width, std::size_t height);

        /// @brief function that will allocate in a similar way to the pinned memory, 
        ///         however it only allocates one extra grid and it uses the managed memory allocation mode
        /// @param host_ptr @ref _allocatePinned
        /// @param dev_ptr @ref _allocatePinned 
        /// @param width @ref _allocatePinned
        /// @param height @ref _allocatePinned
        void _allocateManagedAndSync(bool **host_ptr, bool **dev_ptr, std::size_t width, std::size_t height);

        void _copyGridNormal(bool* dev_ptr, bool * host_ptr);
        
        /// @brief cuda memcpy that will sync before copying to the host
        /// @param dev_ptr the device memory address to copy from 
        /// @param host_ptr the host memory address to copy to
        void _copyGridPinned(bool* dev_ptr, bool * host_ptr);

        /// @brief reverse of @ref _copyGridPinned without the sync 
        /// @param host_ptr 
        /// @param dev_ptr 
        void _copyGridNormalToDev(bool *host_ptr, bool *dev_ptr);
        
    private:
        int _mode;
        bool* _displayMatrix;
        bool* _displayMatrixTest;
        bool* _devicePtrPrevState;
        bool* _devicePtrCurrenState;
        dim3 _blockSize, _gridSize;
        std::size_t _widthSize, _heightSize, _numThreads;
};
#endif // __GAMEOFLIFE_H__