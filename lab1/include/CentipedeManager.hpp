/*
Author: Ben Hall
Class: ECE6122
Last Date Modified: 9/30/24
Description: centipede manager header
*/

#ifndef __CENTIPEDEMANAGER_H__
#define __CENTIPEDEMANAGER_H__

#include <ECE_Centipede.hpp>
#include <vector>

class CentipedeManager
{
public:
    CentipedeManager();
    std::vector<ECE_Centipede>& getSegments() { return _segments; } 
    void initSegments();
    void evaluateSegments(std::vector<sf::Vector2f> mushroomPositions, float deltaTime);
    void handleSegmentImpact(size_t impactIndex);
    bool evaluateLaser(sf::Vector2f laserPosition, int &currentScore);
    void evaluateMushroomLocations(std::vector<sf::Vector2f> mushroomPositions);
    void evaluateHeadPositions();
    void drawOnWindow(sf::RenderWindow & window);
private:
    ECE_Centipede _body, _head;
    sf::Vector2f _normalize(sf::Vector2f v);
    std::vector<ECE_Centipede> _segments;
    std::vector<sf::Vector2f> _prevVelocities;
    std::vector<sf::Vector2f> _prevPositions;
};

#endif // __CENTIPEDEMANAGER_H__