/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 11/08/24
Description: file containing the implementation of the GoL class that wraps the CUDA functions implemented
*/

#include <GameOfLife.hpp>
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <cmath>

#include <algorithm>    // std::swap

#define DEBUG true
inline
cudaError_t checkCuda(cudaError_t result)
{
#if defined(DEBUG) || defined(_DEBUG)
  if (result != cudaSuccess) {
    fprintf(stderr, "CUDA Runtime Error: %s\n", 
            cudaGetErrorString(result));
    assert(result == cudaSuccess);
  }
#endif
  return result;
}

GoL::GoL(std::size_t width, std::size_t height, std::size_t n, int cudaMode)
{
    // row by col: height by width
    
    _heightSize = height;
    _widthSize = width;
    _mode = cudaMode;
    _numThreads = n;
    auto b = sqrt(_numThreads);
    dim3 blockSize(b, b);  // block of threads
    dim3 gridSize((_widthSize + blockSize.x - 1) / blockSize.x, (_widthSize + blockSize.y - 1) / blockSize.y);
    _blockSize = blockSize;
    _gridSize = gridSize;

    
    _displayMatrixTest = static_cast<bool *>(malloc(sizeof(bool)*height*width));
    switch(cudaMode)
    {
        case 0:
        {
            _displayMatrix = static_cast<bool *>(malloc(sizeof(bool)*height*width));
            _generateGrid(width, height, _displayMatrix);
            _allocateNormal(&_devicePtrCurrenState, width, height);
            _allocateNormal(&_devicePtrPrevState, width, height);
            _copyGridNormalToDev(_displayMatrix, _devicePtrCurrenState);
            std::cout << "copied grid prev" <<std::endl;
            _copyGridNormalToDev(_displayMatrix, _devicePtrPrevState);
            std::cout << "copied grid curr" <<std::endl;
            break;
        }
        case 1:
        {
            _allocatePinned(&_displayMatrix, &_devicePtrCurrenState, &_devicePtrPrevState, width, height);
            _generateGrid(width, height, _displayMatrix);
            
            _copyGridNormalToDev(_displayMatrix, _devicePtrCurrenState);
            _copyGridNormalToDev(_displayMatrix, _devicePtrPrevState);
            break;
        }
        case 2:
        {
            // turns the prev device ptr into a managed one that we can use as well
            _allocateManagedAndSync(&_displayMatrix, &_devicePtrPrevState, width, height);
            _generateGrid(width, height, _displayMatrix);
            _generateGrid(width, height, _devicePtrPrevState);
        }
        default:
        {
            break;
        }
    }
}

GoL::~GoL()
{
    free(_displayMatrixTest);
    switch(_mode)
    {
        case 0:
        {
            free(_displayMatrix);
            _deallocNormal(_devicePtrCurrenState);
            _deallocNormal(_devicePtrPrevState);
            break;
        }
        case 1:
        {
            _deallocPinned(_displayMatrix);
            _deallocNormal(_devicePtrCurrenState);
            _deallocNormal(_devicePtrPrevState);
            break;
        }
        case 2:
        {
           _deallocNormal(_displayMatrix); 
           _deallocNormal(_devicePtrPrevState);
        }
        default:
        {
            break;
        }
    } 
}

void GoL::_allocateNormal(bool **dev_ptr, std::size_t width, std::size_t height)
{
    checkCuda(cudaMalloc((void **)dev_ptr, height * width * sizeof(bool)));
}

void GoL::_allocatePinned(bool **host_ptr, bool **dev_ptr, bool **other_dev_ptr, std::size_t width, std::size_t height)
{
    checkCuda(cudaMallocHost((void**)host_ptr, height * width * sizeof(bool)));
    checkCuda(cudaMalloc((void **)dev_ptr, height * width * sizeof(bool)));
    checkCuda(cudaMalloc((void **)other_dev_ptr, height * width * sizeof(bool)));
}

void GoL::_allocateManagedAndSync(bool **host_ptr, bool **dev_ptr, std::size_t width, std::size_t height)
{
    checkCuda(cudaMallocManaged((void**)host_ptr, height * width * sizeof(bool)));
    checkCuda(cudaMallocManaged((void **)dev_ptr, height * width * sizeof(bool)));
    cudaDeviceSynchronize();
}

void GoL::_deallocNormal(bool *dev_ptr)
{
    cudaFree(dev_ptr);
}

void GoL::_deallocPinned(bool *host_ptr)
{
    cudaFreeHost(host_ptr);
}

void GoL::_copyGridNormal(bool *dev_ptr, bool *host_ptr) {
    if(!dev_ptr || !host_ptr)
    {
        if(!dev_ptr)
        {
            std::cout << "WARNING: dev ptr null" <<std::endl;
        }
        if(!host_ptr)
        {
            std::cout << "WARNING: host ptr null" <<std::endl;
        }
        return;
    }
    // Copy the device pointer array to host first
    cudaMemcpy(host_ptr, dev_ptr, sizeof(bool) * _heightSize * _widthSize, cudaMemcpyDeviceToHost);
}

void GoL::_copyGridPinned(bool *dev_ptr, bool *host_ptr) {
    if(!dev_ptr || !host_ptr)
    {
        if(!dev_ptr)
        {
            std::cout << "WARNING: dev ptr null" <<std::endl;
        }
        if(!host_ptr)
        {
            std::cout << "WARNING: host ptr null" <<std::endl;
        }
        return;
    }
    // Copy the device pointer array to host first
    cudaDeviceSynchronize();
    cudaMemcpy(host_ptr, dev_ptr, sizeof(bool) * _heightSize * _widthSize, cudaMemcpyDeviceToHost);
}

void GoL::_copyGridNormalToDev(bool *host_ptr, bool *dev_ptr) {
    if(!dev_ptr || !host_ptr)
    {
        if(!dev_ptr)
        {

            std::cout << "WARNING: dev ptr null" <<std::endl;
        }
        if(!host_ptr)
        {
            std::cout << "WARNING: host ptr null" <<std::endl;
        }
        return;
    }
    // Copy the device pointer array to host first
    cudaMemcpy(dev_ptr, host_ptr, sizeof(bool) * _heightSize * _widthSize, cudaMemcpyHostToDevice);
}

bool GoL::_generateGrid(std::size_t width, std::size_t height, bool *grid)
{
    if(!grid)
    {
        return false;
    }

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (std::size_t x = 0; x < width; ++x)
    {
        for (std::size_t y = 0; y < height; ++y)
        {
            grid[x + (y * width)] = (std::rand() % 2 == 0); // Randomly seed each pixel
        }
    }
    return true;
}

bool * GoL::getGrid()
{
    // _updateGridTest(_displayMatrixTest, _displayMatrix, _widthSize, _heightSize);
    // std::swap(_displayMatrix, _displayMatrixTest);
    switch(_mode)
    {
        case 0:
        case 1:
        {
            updateGrid<<<_gridSize, _blockSize>>>( _devicePtrCurrenState, _devicePtrPrevState, _widthSize, _heightSize);
            _copyGridNormal(_devicePtrCurrenState, _displayMatrix);
            std::swap(_devicePtrCurrenState, _devicePtrPrevState);
            break;
        }
        case 2:
        {
            updateGrid<<<_gridSize, _blockSize>>>(_displayMatrix, _devicePtrPrevState, _widthSize, _heightSize);
            cudaDeviceSynchronize();
            std::swap(_displayMatrix, _devicePtrPrevState);
            break;
        }
        default:
        {
            break;
        }
    }
    
    return _displayMatrix;
}

// test functions for 1d to 2d un-flattened test
int GoL::_countNeighborsTest(bool *state, int x, int y, std::size_t width, std::size_t height)
{
    int neighbors = 0;
    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            if (dx == 0 && dy == 0) continue; 

            int nx = (x + dx + width) % width; 
            int ny = (y + dy + height) % height; 

            neighbors += state[nx + (ny * width)]; 
        }
    }
    return neighbors;
}

void GoL::_updateGridTest(bool *current_state, bool *prev_state, std::size_t width, std::size_t height)
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int neighbors = _countNeighborsTest(prev_state, x, y, width, height);
            int index = x + (y * width);
            
            if (prev_state[index])
            {
                if (neighbors < 2 || neighbors > 3)
                {
                    current_state[index] = false;
                }
                else
                {
                    current_state[index] = prev_state[index];
                }
            }
            else
            {
                if (neighbors == 3)
                {
                    current_state[index] = true;
                }
                else
                {
                    current_state[index] = false;
                }
            }
        }
    }
}