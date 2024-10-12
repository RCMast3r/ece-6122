#include <string>
#include <array>
#include <GameOfLife.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

const int windowWidth = 800;
const int windowHeight = 600;
int numThreads = 8;
int cellSize = 5;

std::string threadingHandler = "THRD";
constexpr int gridWidth = windowWidth / 5;
constexpr int gridHeight = windowHeight / 5;
GameOfLife<gridWidth, gridHeight> game;
std::array<std::array<bool, gridHeight>, gridWidth> grid, prevGrid;


uint64_t count = 0;
int main(int agrc, char *argv[])
{
    auto & gridRef = grid;
    auto & prevGridRef = prevGrid;
    
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Game of Life");
    window.setFramerateLimit(120); // Set frame rate to control speed
    
    game.init(prevGrid);
    sf::Event event;
    // auto refs = game.getRefs();
    // auto & prevRef = refs.first;
    // auto & currentRef = refs.second;
    while (window.isOpen())
    {
        
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
        // every second time, we get update the current referrence 
        // with the reference to the one that will be drawn. the refnext is ref to the
        if (count++ % 2)
        {
            prevGridRef = grid;
            gridRef = prevGrid;
        }

        game.updateGrid(prevGridRef, gridRef);
        window.clear();
        
        for (int x = 0; x < gridWidth; ++x)
        {
            for (int y = 0; y < gridHeight; ++y)
            {
                if (gridRef[x][y])
                {
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