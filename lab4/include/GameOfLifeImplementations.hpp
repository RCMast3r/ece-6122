/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 11/08/24
Description: extern header for the global updateGrid function
*/

#ifndef __GAMEOFLIFENORMAL_H__
#define __GAMEOFLIFENORMAL_H__
#include <cuda.h>
#include <cuda_runtime.h>
#include <cstddef>

__global__ extern void updateGrid(bool *current_state, bool *prev_state, int width, int height);

#endif