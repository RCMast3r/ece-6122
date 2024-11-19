#include <SFML/Graphics.hpp>

#include <thread>
#include <chrono>

#include <server.hpp>


int main()
{
    using namespace std::chrono_literals;

    Server test_server(54000);
    std::size_t width = 800;
    std::size_t height = 600;
    sf::RenderWindow window(sf::VideoMode(width, height), "server");

    window.setFramerateLimit(120); // Set frame rate to control speed
    sf::Event event; 

    int cell_size = 30;
    int initPositionX = (static_cast<int>(width) / 2) - (cell_size/2);
    int initPositionY = (static_cast<int>(height) / 2) - (cell_size/2);
    while(true)
    {
        test_server.poll();
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                test_server.sendExitNotification();
                window.close();
                
                return 0;
            }
            else if (event.type == sf::Event::KeyPressed &&
                     (event.key.code == sf::Keyboard::Escape))
            {
                test_server.sendExitNotification();
                window.close(); // detect the escape key press and if so exit
                return 0;
            }
        }
        sf::RectangleShape cell(sf::Vector2f(cell_size, cell_size));
        cell.setPosition(initPositionX, initPositionY);
        cell.setFillColor(sf::Color::White);
        window.draw(cell);
        window.display();
    }
    return 0;
}