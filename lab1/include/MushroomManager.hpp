#ifndef __MUSHROOMMANAGER_H__
#define __MUSHROOMMANAGER_H__

#include <list>

#include <SFML/Graphics.hpp>

// user story:
//  I want this class to manage the states of the mushrooms (creation, texture update, etc.)

// requirements:

// "Use the random number generator (std::uniform_int_distribution) to randomly locate 30
//  mushrooms in the main game play area. Use a std::list to maintain the state and location
//  of the mushrooms. Feel free to use either a class or struct to hold the information."


/// @brief mushroom manager for managing locations and state of the mushrooms
class MushroomManager
{
    public:
        struct MushroomState
        {
            unsigned int x;
            unsigned int y;
            unsigned int state; // 0 = full health, 1 = degraded state
        };
        
        MushroomManager(unsigned int mushroomAreaWidth, unsigned int mushroomAreaHeight, size_t numMushrooms, bool &construction_failure);
        std::list<MushroomState>& getMushroomStates() {return _states; };
        sf::Sprite makeMushroomGraphic(MushroomState state);

    private:
        sf::Texture _commonFirstStageTexture;
        sf::Texture _commonSecondStageTexture;
        std::list<MushroomState> _states;
};
#endif // __MUSHROOMMANAGER_H__