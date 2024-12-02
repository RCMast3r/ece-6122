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
    auto opts = getConfig(argc, argv);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    MPI_Init(NULL, NULL);
    int worldRank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);


    auto xVals = std::vector<double>();
    if (opts.numSamples > 0) xVals.resize(opts.numSamples);
    if (worldRank == 0) {
        for (std::size_t n = 0; n < opts.numSamples; n++) {
            xVals.at(n) = dis(gen);
        }
    }

        // Broadcast size and data
    int size = static_cast<int>(opts.numSamples);
    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (worldRank != 0) xVals.resize(size);
    MPI_Bcast(xVals.data(), size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double sub_avg = 0.0;
    switch (opts.integralVersion) {
        case IntegralType::PRIMARY:
            sub_avg = getResult(computePrimaryIntegral, xVals, worldRank, worldSize);
            break;
        case IntegralType::SECONDARY:
            sub_avg = getResult(computeSecondaryIntegral, xVals, worldRank, worldSize);
            break;
        default:
            sub_avg = getResult(computePrimaryIntegral, xVals, worldRank, worldSize);
    }

    double total_avg = 0.0;
    MPI_Reduce(&sub_avg, &total_avg, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (worldRank == 0) {
        std::cout << "Estimate for integral " << static_cast<int>(opts.integralVersion)
                  << " is " << (total_avg / worldSize) << std::endl;
    }

    MPI_Finalize();
    return 0;
}
