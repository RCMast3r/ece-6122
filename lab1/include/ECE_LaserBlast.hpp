#ifndef __ECE_LASERBLAST_H__
#define __ECE_LASERBLAST_H__

#include <SFML/Graphics.hpp>
#include <StarShip.hpp>
#include <MushroomManager.hpp>
// #include <ECE_centipede.hpp>

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

    ECE_LaserBlast(unsigned int startX, unsigned int startY, float vel)
    {
        sf::Texture blast;
        blast.create(3, 10);
        setTexture(blast);
        setColor({200, 0, 0});
        setPosition(startX, startY);
        _vel = vel;
    }

    // returns true if the
    BlastState getState(float deltaTime, std::list<MushroomManager::MushroomState>& mushroomStates, std::vector<sf::Vector2f> centipedePositions, int& currentScore);

private:
    BlastState _state;
    float _vel;
};

#endif // __ECE_LASERBLAST_H__