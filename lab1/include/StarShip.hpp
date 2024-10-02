/*
Author: Ben Hall
Class: ECE6122
Last Date Modified: 9/30/24
Description: Starship class for calculation of the state of the starship
*/

#ifndef __STARSHIP_H__
#define __STARSHIP_H__

#include <SFML/Graphics.hpp>
#include <Spider.hpp>

/// @brief starship class managing
class StarShip
{
public:
    struct StarShipControlInput
    {
        int leftInput;
        int rightInput;
        int upInput;
        int downInput;
        bool isFiring;
    };

    struct StarShipState
    {
        bool isDead;
        bool emitLaser;
        unsigned int centerPosX;
        unsigned int centerPosY;
        sf::Texture texture;
        sf::Sprite sprite;
        size_t liveCount;
    };

    /// @brief constructor
    /// @param velocity 
    /// @param construction_error
    /// @param assets_path
    explicit StarShip(unsigned int starshipWindowWidth, unsigned int starshipWindowHeight, unsigned int heightOffset, float velocity, bool &construction_error, std::string asset_path = "graphics/StarShip.png");

    // delete the default constructor since we dont need it or what it to be a thing
    StarShip() = delete;
    // TODO pass in the states of the mushrooms to block movement into mushroom
    void command(StarShipControlInput input, float deltaTime, Spider::SpiderState spiderState, Spider &spider);
    StarShipState getCurrentState() { return _state; }
    void resetDeath()
    {
        _state.isDead = false;
        _state.liveCount = 3;
    }

private:
    unsigned int _windowWidth, _windowHeight, _heightOffset;
    StarShipState _state;
};

#endif // __STARSHIP_H__