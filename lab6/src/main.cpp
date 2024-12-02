/*
Author: Ben Hall
Class: ECE6122 
Last Date Modified: 12/2/24
Description: lab6 complete program containing all functions including main

Implements lab6 for estimating 2 different integrals for e^(-x^2) and x^2 for x=1
*/
#include <cmath>
#include <cstddef>
#include <iostream>
#include <mpi.h>
#include <random>
#include <vector>
#include <numeric>
#include <functional>

enum class IntegralType { PRIMARY = 1, SECONDARY = 2 };

/// @brief all program options
struct ProgramOptions {
  std::size_t numSamples = 1000000;
  IntegralType integralVersion = IntegralType::PRIMARY;
};

/// @brief the 
/// @param argc 
/// @param argv 
/// @return 
ProgramOptions getConfig(int argc, char *argv[]) {
    ProgramOptions options;
    for (int i = 1; i < argc; i++) {
        if ((std::string(argv[i]) == "-P") && ((i + 1) < argc)) {
            auto ver = std::stoi(std::string(argv[i + 1]));
            options.integralVersion =
                static_cast<IntegralType>(std::stoi(std::string(argv[i + 1])));

            
        } else if ((std::string(argv[i]) == "-N") && ((i + 1) < argc)) {
            options.numSamples = std::stoi(std::string(argv[i + 1]));
        }
    }
    return options;
}

/// @brief x^2 
/// @param x x
/// @return f(x)
double computePrimaryIntegral(double x) { return x * x; }

/// @brief e^(-x^2)
/// @param x x
/// @return f(x)
double computeSecondaryIntegral(double x) { return ::exp(-1 * ::pow(x, 2)); }

/// @brief average result calculator
/// @param func the function to use for estimation of the integral
/// @param vals values to calculate from
/// @param rank the index of this task / id
/// @param size the number of tasks 
/// @return the average result
double getResult(std::function<double(double)> func, const std::vector<double>& vals, std::size_t rank, std::size_t size)
{
    double length = 0.0f;
    double total = 0.0f;
    for(std::size_t i = 0; i < vals.size(); i ++)
    {
        length++; 
        total+= func(vals[i]);
    }
    return total / length;
}

int main(int argc, char *argv[]) {

    // handle program options
    auto opts = getConfig(argc, argv);

    // set up the random number generator and get values from it
    std::random_device rd; // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0.0, 1.0);

    auto xVals = std::vector<double>(opts.numSamples);
    for (std::size_t n = 0; n < opts.numSamples; n++) {
        xVals.at(n) = dis(gen);
    }

    // initialize MPI 
    MPI_Init(NULL, NULL);

    // get the current task index and total number of world tasks
    int worldRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    int worldSize;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    // broadcast all data to every task
    MPI_Bcast(static_cast<void *>(xVals.data()), xVals.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    double sub_avg = 0.0f;
    // handle calculation of the results within each process
    switch(static_cast<IntegralType>(opts.integralVersion))
    {
        case IntegralType::PRIMARY:
        {
            sub_avg = getResult(computePrimaryIntegral, xVals, worldRank, worldSize);
            break;
        }
        case IntegralType::SECONDARY:
        {
            sub_avg = getResult(computeSecondaryIntegral, xVals, worldRank, worldSize);
            break;
        }
        default:
        {
            sub_avg = getResult(computePrimaryIntegral, xVals, worldRank, worldSize);
            break;
        }
    }

    // send from all processes the sub- average to be averaged
    MPI_Send(static_cast<void *>(&sub_avg), 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

    std::vector<double> results;
    if(worldRank == 0)
    {
        results.resize(worldSize);
    }
    // gather the results from the all tasks, including the root one.
    MPI_Gather(static_cast<void *>(&sub_avg), 1, MPI_DOUBLE, static_cast<void*>(results.data()), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // get the final average and print out the result 
    if (worldRank == 0) {
        double sum =0;
        for(const auto & r : results)
        {
            sum+=r;
        }

        double avg = sum / static_cast<double>(results.size());
        std::cout << "The estimate for integral "<< static_cast<int>(opts.integralVersion) <<" is " << avg <<std::endl;
        std::cout << "Bye!" <<std::endl;

    }
    
    // Finalize the MPI environment.
    MPI_Finalize();
    return 0;
}