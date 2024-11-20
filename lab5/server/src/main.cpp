/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 11/19/24
Description: main file for the main() function. 
Also contains the commandline argument handling and terminal, window creation, command responses and velocity calculation for the "robot"
*/

#include <SFML/Graphics.hpp>

#include <thread>
#include <chrono>
#include <iostream>

#include <server.hpp>

#include <cmath>

/// @brief function for handling the input velocity command
/// @param velCmd input command
/// @param curXVel current velocity (x) that will be changed
/// @param curYVel current velocity (y) that will be chagned
/// @param speed the current speed
void handleVelCmd(Server::Direction velCmd, int &curXVel, int &curYVel, int speed)
{
    switch(velCmd){
        case(Server::Direction::DOWN):
        {
            curXVel = 0;
            curYVel = speed;
            break;
        }
        case(Server::Direction::UP):
        {
            curXVel = 0;
            curYVel = -speed;
            break;
        }
        case(Server::Direction::LEFT):
        {
            curXVel = -speed;
            curYVel = 0;
            break;
        }
        case(Server::Direction::RIGHT):
        {
            curXVel = speed;
            
            curYVel = 0;
            break;
        }
        default:
        {
            break;
        }
    }
}
/// @brief function for increasing / decreasing the speed based on the input command
/// @param velCmd current speed change command
/// @param speed the current speed
/// @return new speed
int handleVelChangeCmd(Server::SpeedChange velCmd, int speed)
{
    switch(velCmd){
        case(Server::SpeedChange::INC):
        {
            return speed + 3;
        }
        case(Server::SpeedChange::DEC):
        {
            return speed - 3;
        }
        default:
        {
            return speed;
        }
    }
}

/// @brief main
/// @param argc argument count 
/// @param argv argument vector
/// @return 
int main(int argc, char *argv[])
{
    using namespace std::chrono_literals;
    uint16_t port = 54000;
    if(argc >1)
    {
        port = std::stoi(argv[1]);
    }
    Server server(port);
    std::size_t width = 800;
    std::size_t height = 600;
    sf::RenderWindow window(sf::VideoMode(width, height), "server");
    unsigned int fps = 120;
    window.setFramerateLimit(fps); // Set frame rate to control speed
    sf::Event event; 

    int cell_size = 30;
    int initPositionX = (static_cast<int>(width) / 2) - (cell_size/2);
    int initPositionY = (static_cast<int>(height) / 2) - (cell_size/2);

    int curX = initPositionX;
    int curY = initPositionY;

    int currentSpeed = 3;

    int velX = 0;
    int velY = 0;
    bool robotPresent = true;
    while(true)
    {

        auto msg = server.poll();
        if(msg)
        {
            handleVelCmd((*msg).dirEnum, velX, velY, currentSpeed);
            currentSpeed = handleVelChangeCmd((*msg).velChange, currentSpeed);
            robotPresent = !(*msg).disappear;
        }
        window.clear();

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                server.sendExitNotification();
                window.close();
                return 0;
            }
            else if (event.type == sf::Event::KeyPressed &&
                     (event.key.code == sf::Keyboard::Escape))
            {
                server.sendExitNotification();
                window.close(); // detect the escape key press and if so exit
                return 0;
            }
        }
        sf::RectangleShape cell(sf::Vector2f(cell_size, cell_size));
        
        float frameSpeed = static_cast<float>(currentSpeed) / 120.0f;

        // Compute the movement increment for X and Y
        float moveX = std::abs(static_cast<float>(velX)) * frameSpeed;
        float moveY = std::abs(static_cast<float>(velY)) * frameSpeed;

        // Ensure the movement is at least 1 pixel
        int deltaX = 0;
        if(::abs(velX))
        {
            deltaX = std::max(1, static_cast<int>(std::ceil(moveX)));
        }

        int deltaY = 0;
        if(::abs(velY))
        {
            deltaY = std::max(1, static_cast<int>(std::ceil(moveY)));
        }
        
        // Adjust the position with the proper sign
        curX += deltaX * (std::signbit(velX) ? -1 : 1);
        curY += deltaY * (std::signbit(velY) ? -1 : 1);

        if(curX > (width - (cell_size/2)))
        {
            curX = (width - (cell_size/2));
        } else if(curX < 0)
        {
            curX = 0;
        }
        
        if(curY > (height - (cell_size/2)))
        {
            curY = (height - (cell_size/2));
        } else if(curY < 0)
        {
            curY = 0;
        }
        if(robotPresent)
        {
            cell.setPosition(curX, curY);
            cell.setFillColor(sf::Color::White);
            window.draw(cell);
        }
        
        window.display();
    }
    return 0;
}