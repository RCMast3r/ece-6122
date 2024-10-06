/*
Author: Ben Hall
Class: ECE6122
Last Date Modified: 9.25/24
Description: top-level main with run-time loop
*/


// game logic:
// - grid of specific size: 27x27 pixels to fit grid of mushrooms 
// - starts with 30 mushrooms in random areas
// space-ship:
// - can move around with arrow keys in limited area of the screen (bottom 1/3rd of the play window or so) with space to fire the laser
// lives counter: 3 lives to start out with -> 
// - when you collide with any part of the centipede you die
// - when you collide with the spider, you die

// centipede logic:
// - centipede starts out as a head with 11 body sections
// - when any section (body or head) of the centipede gets hit, it gets changed into a mushroom at that position
// - if the head of the centipede gets hit, the next body section gets promoted to being the head of the centipede. 
//    if the centipede has no body section it dissapears (still leaving a mushroom)
// - 

// TODOs:
// - [x] main start screen w/ pressing enter to start the game
// - [x] space-ship entity logic
// - [ ] centipede class for calculating new location based -> tick system based on current time and fixed velocity 
// - [x] laser blast -> may want to 
// - [ ] game window / screens
//      -[ ] add in life counter that uses spaceship graphic
// - [ ] ensure that every file has the assignment header at the top of it
// - [ ] ensure that every variable name and function is camel case
// - [ ] make the spider die and respawn on kill
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <GameLogicManager.hpp>
// #include <GameUtils.hpp>
int main()
{
    bool startup_failed;
    GameLogicManager game(startup_failed);
    if(startup_failed)
        return EXIT_FAILURE;
    
    
    // Load a sprite to display

    
    // Create a graphical text to display
    // Start the game loop
    sf::Clock clock;

    while (game.alive())
    {
        sf::Time elapsedTime = clock.restart();
        game.tickGame(elapsedTime.asSeconds());
    }
 
    return EXIT_SUCCESS;
}