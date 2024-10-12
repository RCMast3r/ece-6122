#include <GameOfLife.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <chrono> // std::chrono::seconds
#include <iostream>
#include <string>

const int windowWidth = 800;
const int windowHeight = 600;
int numThreads = 8;
constexpr int cellSize = 5;

std::string threadingHandler = "THRD";
constexpr int gridWidth = windowWidth / cellSize;
constexpr int gridHeight = windowHeight / cellSize;
GameOfLife<gridWidth, gridHeight> game(1);
std::array<std::array<bool, gridHeight>, gridWidth> grid, prevGrid;

uint64_t count = 0;

struct ProgramOptions
{

};

// void handleCMDArgs(std::string &threadingHandlerMode, int& cellSize, int& windowWidth, int& windowHeight)

int main(int agrc, char *argv[]) {
    auto &gridRef = grid;
    auto &prevGridRef = prevGrid;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight),
                            "Game of Life");
    window.setFramerateLimit(120); // Set frame rate to control speed

    game.init();
    sf::Event event;
    int generationCount = 0;
    unsigned long elapsedMicros = 0;
    while (window.isOpen()) {

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        auto start_time = std::chrono::high_resolution_clock::now();
        game.updateGrid();
        // game.updateGridThreaded();
        // game.updateGridOpenMPThreaded(8);

        auto end_time = std::chrono::high_resolution_clock::now();
        generationCount++;
        if (generationCount == 100) {
            std::cout << "100 generations took " << elapsedMicros
                      << " microseconds with single thread." << std::endl;
            generationCount = 0;
            elapsedMicros = 0;
        } else {
            elapsedMicros +=
                std::chrono::duration_cast<std::chrono::microseconds>(
                    end_time - start_time)
                    .count();
        }
        window.clear();
        auto refs = game.getRefs();
        for (int x = 0; x < gridWidth; ++x) {
            for (int y = 0; y < gridHeight; ++y) {
                if (refs.first[x][y]) {
                    sf::RectangleShape cell(sf::Vector2f(5, 5));
                    cell.setPosition(x * 5, y * 5);
                    cell.setFillColor(sf::Color::White);
                    window.draw(cell);
                }
            }
        }
        window.display();
    }
    return 0;
}