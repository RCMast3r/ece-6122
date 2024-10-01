#ifndef __CENTIPEDEMANAGER_H__
#define __CENTIPEDEMANAGER_H__

#include <ECE_Centipede.hpp>
#include <vector>

class CentipedeManager
{
public:
    CentipedeManager();
    std::vector<ECE_Centipede> getSegments() {return _segments; } 
    void evaluateSegments(std::vector<sf::Vector2f> mushroomPositions, float deltaTime);
private:
    ECE_Centipede _body, _head;
    sf::Vector2f _normalize(sf::Vector2f v);
    std::vector<ECE_Centipede> _segments;
    std::vector<sf::Vector2f> _prevVelocities;
    std::vector<sf::Vector2f> _prevPositions;
};

#endif // __CENTIPEDEMANAGER_H__