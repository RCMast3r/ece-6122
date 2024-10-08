/*
Author: Ben Hall
Class: ECE6122
Last Date Modified: 9/30/24
Description: required ECE_LaserBlast class description. Handles interaction with entities
*/

#ifndef __ECE_LASERBLAST_H__
#define __ECE_LASERBLAST_H__

#include <SFML/Graphics.hpp>
#include <StarShip.hpp>
#include <MushroomManager.hpp>
#include <Spider.hpp>
#include <GameUtils.hpp>
#include <CentipedeManager.hpp>

// requirements:
// Create an ECE_LaserBlast class derived from the SFML::Sprite that calculates the
// current location of the laser blast and detecting collisions with objects and taking the
// appropriate action
class ECE_LaserBlast : sf::Sprite
{
public:
    struct BlastState
    {
        bool impacted;
    };
    /// @brief constructor
    /// @param startX 
    /// @param startY 
    /// @param vel 
    ECE_LaserBlast(unsigned int startX, unsigned int startY, float vel)
    {
        sf::Texture blast;
        blast.create(3, 10);
        setTexture(blast);
        setColor({200, 0, 0});
        setPosition(startX, startY);
        _vel = vel;
    }
    /// @brief handles the evaluation of the state of the blast and can change the state of the arguments passed in by referrence
    /// @param deltaTime the current time delta
    /// @param mushroomStates the states of the mushrooms
    /// @param centipede the centipede manager
    /// @param spider the spider that can be reset / killed
    /// @param currentScore score incrementing
    /// @return the resulting state of the blast (whether or not to draw it or erase it)
    BlastState getState(float deltaTime, std::list<MushroomManager::MushroomState>& mushroomStates, CentipedeManager& centipede, Spider &spider, int& currentScore);

private:
    BlastState _state;
    float _vel;
};

#endif // __ECE_LASERBLAST_H__