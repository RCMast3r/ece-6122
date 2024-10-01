#include <CentipedeManager.hpp>
#include <cmath>

CentipedeManager::CentipedeManager() :
    _head(true),
    _body(false)
    {
    _segments.push_back(_head);
    auto curPos = _head.getPosition();

    for(int i=0; i<11; i++)
    {
        curPos.x -= 24;
        _body.setPosition({curPos.x, curPos.y});
        _segments.push_back(_body);
    }
}

float distance(sf::Vector2f a, sf::Vector2f b) {
    return std::sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

sf::Vector2f CentipedeManager::_normalize(sf::Vector2f v) {
    float mag = std::sqrt(v.x * v.x + v.y * v.y);
    if (mag == 0) return sf::Vector2f(0, 0);
    return sf::Vector2f(v.x / mag, v.y / mag);
}

void CentipedeManager::evaluateSegments(std::vector<sf::Vector2f> mushroomPositions, float deltaTime)
{
    size_t bodyInd = 0;
    size_t headInd = 0;
    size_t ind = 0;
    auto prevPosition = _segments[0].getPosition();
    _segments[0].evaluateHeadPosition(mushroomPositions, deltaTime);
    sf::Vector2f prev2Position;
    for (int i = 1; i <= 11; ++i) {
        prev2Position = _segments[i].getPosition();
        _segments[i].move({(prevPosition.x-prev2Position.x)/12, (prevPosition.y-prev2Position.y)/12});
        prevPosition = prev2Position;
    }
    
}