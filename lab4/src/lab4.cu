/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 11/08/24
Description: main file for the main() function. 
Also contains the commandline argument handling function and default args to pass to the game class.
Contains the class and window instances
*/

#include <memory>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <chrono> // std::chrono::seconds
#include <GameOfLife.hpp>
// 3 different modes being evaluated:

// 1 normal
// normal malloc and copy that requires copying between host and device

// 2 pinned
// pinned memory: memory allocated using cudamallochost and for allocating host memory that will be given via an async cuda memcpy call
// 3 managed
// memory that is automagically copied back and forth between host and device
// 

/// @brief program options struct that by default holds the default args
struct ProgramOptions
{
    int numThreads = 32;
    int cellSize = 5;
    int windowWidth = 800;
    int windowHeight = 600;
    int CUDAMode = 0;
};

/// @brief function that change the default options configured in the program
/// options struct instance above
/// @param options the ref to the options struct in the main function
/// @param argc number of commandline args
/// @param argv the args themselves
void handleCMDArgs(ProgramOptions &options, int argc, char *argv[])
{

    for (int i = 1; i < argc; i++)
    {
        if ((std::string(argv[i]) == "-n") && ((i + 1) < argc))
        {
            options.numThreads = std::stoi(std::string(argv[i + 1]));
        }
        else if ((std::string(argv[i]) == "-c") && ((i + 1) < argc))
        {
            options.cellSize = std::stoi(std::string(argv[i + 1]));
        }
        else if ((std::string(argv[i]) == "-x") && ((i + 1) < argc))
        {
            options.windowWidth = std::stoi(std::string(argv[i + 1]));
        }
        else if ((std::string(argv[i]) == "-y") && ((i + 1) < argc))
        {
            options.windowHeight = std::stoi(std::string(argv[i + 1]));
        }
        else if ((std::string(argv[i]) == "-t") && ((i + 1) < argc))
        {
            if (std::string(argv[i + 1]) == "NORMAL")
            {
                options.CUDAMode = 0;
            }
            else if (std::string(argv[i + 1]) == "PINNED")
            {

                options.CUDAMode = 1;
            }
            else if (std::string(argv[i + 1]) == "MANAGED")
            {

                options.CUDAMode = 2;
            }
            else
            {
                std::cout << "WARNING: threading model type unknown, "
                             "defaulting to NORMAL"
                          << std::endl;
            }
        }
    }
}

/// @brief main function
/// @param argc arg count
/// @param argv args
/// @return 
int main(int argc, char *argv[])
{
    ProgramOptions options;
    handleCMDArgs(options, argc, argv);
    const int gridWidth = options.windowWidth / options.cellSize;
    const int gridHeight = options.windowHeight / options.cellSize;

    // create the game instance with the specified mode and thread count
    GoL game(gridWidth, gridHeight, options.numThreads, options.CUDAMode);

    sf::RenderWindow window(sf::VideoMode(options.windowWidth, options.windowHeight), "Game of Life");
    
    window.setFramerateLimit(120); // Set frame rate to control speed
    
    sf::Event event;
    int generationCount = 0;
    unsigned long elapsedMicros = 0;
    
    while (window.isOpen())
    {

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed &&
                     (event.key.code == sf::Keyboard::Escape))
            {
                window.close(); // detect the escape key press and if so exit
                continue;
            }
        }

        // timing
        auto start_time = std::chrono::high_resolution_clock::now();
        auto grid = game.getGrid(); // computation happens here
        auto end_time = std::chrono::high_resolution_clock::now();
        generationCount++;

        // required output / print handling
        if (generationCount == 100)
        {
            if (options.CUDAMode == 0)
            {
                std::cout << "100 generations took " << elapsedMicros
                          << " microseconds with " << options.numThreads
                          << " threads per block using Normal memory allocation" << std::endl;
            }
            else if (options.CUDAMode == 1)
            {

                std::cout << "100 generations took " << elapsedMicros
                          << " microseconds with " << options.numThreads
                          << " threads per block using Pinned memory allocation" << std::endl;
            }
            else if (options.CUDAMode == 2)
            {
                std::cout << "100 generations took " << elapsedMicros
                          << " microseconds with " << options.numThreads
                          << " threads per block using Managed memory allocation" << std::endl;
            }
            generationCount = 0;
            elapsedMicros = 0;
        }
        else
        {
            elapsedMicros +=
                std::chrono::duration_cast<std::chrono::microseconds>(
                    end_time - start_time)
                    .count();
        }

        window.clear();
        auto size = options.cellSize;
        for (int x = 0; x < gridWidth; ++x)
        {
            for (int y = 0; y < gridHeight; ++y)
            {
                if (grid[(y*gridWidth) + x])
                {
                    sf::RectangleShape cell(sf::Vector2f(size, size));
                    cell.setPosition(x * size, y * size);
                    cell.setFillColor(sf::Color::White);
                    window.draw(cell);
                }
            }
        }
        window.display();
    }
    

    
}