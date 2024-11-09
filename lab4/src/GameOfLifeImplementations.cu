/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 11/08/24
Description: file containing the global cuda functions that will get run on a cuda device for updating the game of life grid
*/
#include <cuda.h>
#include <cuda_runtime.h>
#include <GameOfLifeImplementations.hpp>
#include <iostream>
#include <stdio.h>

// https://github.com/NVIDIA/cuda-samples/blob/master/Samples/3_CUDA_Features/newdelete/newdelete.cu
// https://stackoverflow.com/questions/12373940/difference-between-global-and-device-functions

/// @brief function to get the number of neighbors around a coordinate 
/// @param state pointer to the grid itself 
/// @param x the width to check 
/// @param y the height to check
/// @param width width of the grid itself
/// @param height height of the grid
/// @return neighbors that are alive around the desired coordinate
__device__ int countNeighbors(bool *state, std::size_t x, std::size_t y, int width, int height)
{
    int neighbors = 0;
    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            if (dx == 0 && dy == 0) continue; 

            int nx = (x + dx + width) % width;
            int ny = (y + dy + height) % height;

            neighbors += state[nx + (ny * width)]; // Add the neighbor's state
        }
    }
    return neighbors;
}

/// @brief function that actually updates the grid. works on the CUDA allocated grids
/// @param current_state the pointer to the current grid
/// @param prev_state pointer to the previous grid
/// @param width width of grid
/// @param height height of grid
__global__ void updateGrid(bool *current_state, bool *prev_state, int width, int height)
{
    int index_x = threadIdx.x;
    int index_y = threadIdx.y;
    
    int stride_x = blockDim.x;
    int stride_y = blockDim.y;

    for (int y = index_y; y < height; y+=stride_y)
    {
        for (int x = index_x; x < width; x+=stride_x)
        {
            int neighbors = countNeighbors(prev_state, (std::size_t)x, (std::size_t)y, width, height);
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
