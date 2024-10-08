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
const int gridWidth = windowWidth / 5;
const int gridHeight = windowHeight / 5;

int main(int agrc, char *argv[])
{
    
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Game of Life");
    window.setFramerateLimit(120); // Set frame rate to control speed
    GameOfLife<gridWidth, gridHeight> game;
    game.init();
    
    // auto & currentGrid = game.updateGrid();
    // while(true)
    // {
    //     // std::this_thread
        
    // }
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                // window.close();
            }
        }
        // every second time, we get update the current referrence 
        // with the reference to the one that will be drawn. the refnext is ref to the
        
        
        // auto & currentGrid = game.updateGrid();        
        window.clear();
        
        for (int x = 0; x < gridWidth; ++x)
        {
            for (int y = 0; y < gridHeight; ++y)
            {
                if (1)
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