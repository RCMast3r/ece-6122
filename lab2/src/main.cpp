/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 10/12/24
Description: main file for the main() function. 
Also contains the commandline argument handling function and default args to pass to the game class.
Contains the class and window instances
*/

#include <GameOfLife.hpp>
#include <SFML/Graphics.hpp>
#include <chrono> // std::chrono::seconds
#include <iostream>
#include <string>

std::string threadingHandler = "THRD";
uint64_t count = 0;

struct ProgramOptions
{
    int numThreads = 8;
    int cellSize = 5;
    int windowWidth = 800;
    int windowHeight = 600;
    int threadingModel = 1;
};


int main(int argc, char *argv[])
{
    ProgramOptions options;
    handleCMDArgs(options, argc, argv);
    const int windowWidth = options.windowWidth;
    const int windowHeight = options.windowHeight;

    int numThreads = options.numThreads;
    const int cellSize = options.cellSize;

    // derive the grid width from the cell size and the specified window width
    const int gridWidth = windowWidth / cellSize;
    const int gridHeight = windowHeight / cellSize;
    // instance of the game
    GameOfLife game(options.threadingModel, numThreads, gridWidth, gridHeight);

    // create the window and set its name
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight),
                            "Game of Life");

    window.setFramerateLimit(120); // Set frame rate to control speed

    // initialize the game, starts threads conditionally / if specified
    game.init();

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
        game.updateGrid();

        auto end_time = std::chrono::high_resolution_clock::now();
        generationCount++;

        // required output / print handling
        if (generationCount == 100)
        {
            if (options.threadingModel == 0)
            {
                std::cout << "100 generations took " << elapsedMicros
                          << " microseconds with a single thread" << std::endl;
            }
            else if (options.threadingModel == 1)
            {

                std::cout << "100 generations took " << elapsedMicros
                          << " microseconds with " << options.numThreads
                          << " std::threads" << std::endl;
            }
            else if (options.threadingModel == 2)
            {
                std::cout << "100 generations took " << elapsedMicros
                          << " microseconds with " << options.numThreads
                          << " OMP threads" << std::endl;
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
        auto refs = game.getRefs();
        for (int x = 0; x < gridWidth; ++x)
        {
            for (int y = 0; y < gridHeight; ++y)
            {
                if (refs.first[x][y])
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
    return 0;
}