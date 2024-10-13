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
    int threadingModel = 1;
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
            if (std::string(argv[i + 1]) == "SEQ") {
                options.threadingModel = 0;
            } else if (std::string(argv[i + 1]) == "THRD") {

                options.threadingModel = 1;
            } else if (std::string(argv[i + 1]) == "OMP") {

                options.threadingModel = 2;
            } else {
                std::cout << "WARNING: threading model type unknown, "
                             "defaulting to THRD"
                          << std::endl;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    ProgramOptions options;
    handleCMDArgs(options, argc, argv);
    const int windowWidth = options.windowWidth;
    const int windowHeight = options.windowHeight;

    int numThreads = options.numThreads;
    const int cellSize = options.cellSize;

    const int gridWidth = windowWidth / cellSize;
    const int gridHeight = windowHeight / cellSize;
    GameOfLife game(options.threadingModel, numThreads, gridWidth, gridHeight);

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

        auto end_time = std::chrono::high_resolution_clock::now();
        generationCount++;
        if (generationCount == 100) {
            if (options.threadingModel == 0) {
                std::cout << "100 generations took " << elapsedMicros
                          << " microseconds with a single thread" << std::endl;
            } else if (options.threadingModel == 1) {

                std::cout << "100 generations took " << elapsedMicros
                          << " microseconds with " << options.numThreads
                          << " std::threads" << std::endl;
            } else if (options.threadingModel == 2) {
                std::cout << "100 generations took " << elapsedMicros
                          << " microseconds with " << options.numThreads
                          << " OMP threads" << std::endl;
            }
            generationCount = 0;
            elapsedMicros = 0;
        } else {
            elapsedMicros +=
                std::chrono::duration_cast<std::chrono::microseconds>(
                    end_time - start_time)
                    .count();
        }
        window.clear();
        auto size = options.cellSize;
        auto refs = game.getRefs();
        for (int x = 0; x < gridWidth; ++x) {
            for (int y = 0; y < gridHeight; ++y) {
                if (refs.first[x][y]) {
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