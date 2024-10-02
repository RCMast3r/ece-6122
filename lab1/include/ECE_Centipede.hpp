/*
Author: Ben Hall
Class: ECE6122
Last Date Modified: 9/30/24
Description: required ECE_Centipede class description. derives from sf::Sprite
*/

#ifndef __ECE_CENTIPEDE_H__
#define __ECE_CENTIPEDE_H__

#include <SFML/Graphics.hpp>
#include <GameUtils.hpp>
#include <memory>
// linked list made from the sprite base class
// responsibilities:
// - [ ] detect collision with entities: (laser positions and starship position)
// - [ ] calculating its own position (if it is a head it is moving, if it is a body it is following)


class ECE_Centipede : public sf::Sprite
{
public:
    ECE_Centipede(bool isHead);
    // void setBodyPosition(sf::Vector2f position);
    void evaluateHeadPosition(std::vector<sf::Vector2f> mushroomPositions, float deltaTime);
    void evaluateBodyPosition(sf::Vector2f prevSegmentPosition, float deltaTime);
    void promoteToHead()
    {
        _isHead = true;
        setTexture(_headTexture, true);
        _handleImpactTurn();
    }
    void handleBump()
    {
        _handleImpactTurn();
    }
    bool isHead() { return _isHead; }
    float getVelocityMag() {return _velocity;}
    sf::Vector2f getVelocity() { return {_velX, _velY}; }
    void setVelocity(sf::Vector2f vel)
    {
        _velX = vel.x;
        _velY = vel.y;
    }

private:
    // float _distance(sf::Vector2f a, sf::Vector2f b);
    void _handleImpactTurn();
    void _updateHeadVelocities(std::vector<sf::Vector2f> mushroomPositions, float deltaTime);

private:
    float _orientationDeg = 0;
    bool _isHead;
    bool _handlingImpact;
    bool _isMovingVertically;
    bool _isMovingDown, _isMovingRight, _isMovingLeft;
    sf::Texture _headTexture, _bodyTexture;
    float _followDistance;
    float _velX, _velY, _prevVerticalStartY, _velocity;
    // pos _start_pos;
};

#endif // __ECE_CENTIPEDE_H__