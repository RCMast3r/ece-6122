#include <GameOfLife.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <chrono> // std::chrono::seconds
#include <iostream>
#include <string>

std::string threadingHandler = "THRD";
uint64_t count = 0;

struct ProgramOptions {
    int numThreads = 8;
    int cellSize = 5;
    int windowWidth = 800;
    int windowHeight = 600;
    std::string threadingModel = "THRD";
};

void handleCMDArgs(ProgramOptions &options, int argc, char *argv[]) {

    for (int i = 1; i < argc; i++) {
        if ((std::string(argv[i]) == "-n") && ((i + 1) < argc)) {
            options.numThreads = std::stoi(std::string(argv[i + 1]));
            std::cout << options.numThreads << std::endl;
        } else if ((std::string(argv[i]) == "-c") && ((i + 1) < argc)) {
            options.cellSize = std::stoi(std::string(argv[i + 1]));
            std::cout << options.cellSize << std::endl;
        } else if ((std::string(argv[i]) == "-x") && ((i + 1) < argc)) {
            options.windowWidth = std::stoi(std::string(argv[i + 1]));
            std::cout << options.windowWidth << std::endl;
        } else if ((std::string(argv[i]) == "-y") && ((i + 1) < argc)) {
            options.windowHeight = std::stoi(std::string(argv[i + 1]));
            std::cout << options.windowHeight << std::endl;
        } else if ((std::string(argv[i]) == "-t") && ((i + 1) < argc)) {
            options.threadingModel = std::string(argv[i + 1]);
            std::cout << options.threadingModel << std::endl;
        }
    }
}

int main(int argc, char *argv[]) {
    ProgramOptions options;
    handleCMDArgs(options, argc, argv);
    const int windowWidth = options.windowWidth;
    const int windowHeight = options.windowHeight;

    int numThreads = 8;
    const int cellSize = 5;
    const int gridWidth = windowWidth / cellSize;
    const int gridHeight = windowHeight / cellSize;
    GameOfLife game(8, gridWidth, gridHeight);

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
        // game.updateGrid();
        game.updateGridThreaded();
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