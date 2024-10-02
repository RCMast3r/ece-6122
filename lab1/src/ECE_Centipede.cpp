#include <ECE_Centipede.hpp>
#include <iostream>
#include <cmath>
// #include <GameUtils.hpp>

ECE_Centipede::ECE_Centipede(bool isHead) : _isHead(isHead)
{
    _followDistance = 24;
    _velocity = 100.0;
    _handlingImpact = false;
    if(!_headTexture.loadFromFile("graphics/CentipedeHead.png"))
    {
        std::cout << "WARNING: head texture not loaded correctly" <<std::endl;
    }
    if(!_bodyTexture.loadFromFile("graphics/CentipedeBody.png"))
    {
        std::cout << "WARNING: head body not loaded correctly" <<std::endl;
    }
    if (isHead)
    {
        _velX = _velocity;
        _velY = 0.0;
        setTexture(_headTexture);
    }
    else
    {
        setTexture(_bodyTexture);
        _velX = 0.0;
        _velY = 0.0;
    }

    setPosition({(float)(1036 / 2), (float)53});
    _isMovingVertically = false;
    _isMovingDown = true;  // start by going down the screen
    _isMovingRight = true; // start by going down the screen
    _isMovingLeft = false;
}

// float ECE_Centipede::_distance(sf::Vector2f a, sf::Vector2f b) {
//     return std::sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
// }

// if this section is a head, update its own position and make all positions of every other non-heads
// an "offset" from the parent's node

// motion:
// the head starts moving right to left, starting from the top-middle of the screen
// each body piece follows it, and a head hits a mushroom or wall it turns down and
// then starts going the opposite direction it was. Once the head has reached the bottom,
// instead of turning down once at an each or mushroom, it starts progressing back up
// to the top of the screen.

void ECE_Centipede::_updateHeadVelocities(std::vector<sf::Vector2f> mushroomPositions, float deltaTime)
{
    // if the head has reached a wall and we have yet to start moving down, start moving vertically
    if (_isHead)
    {
        auto posX = getPosition().x;
        auto posY = getPosition().y;
        auto predictedX = (posX + 24 + (deltaTime * _velX));
        auto predictedY = (posY + 24 + (deltaTime * _velY));
        if (((predictedX >= 1020) || (predictedX <= 24)) && !_isMovingVertically)
        {
            _isMovingVertically = true;
            _prevVerticalStartY = getPosition().y;
            _velX = 0; // stop horizontal motion
            _isMovingDown = ((predictedY < 400) && (_isMovingDown)) || (!_isMovingDown && (predictedY < 80));
            _velY = _isMovingDown ? _velocity : -_velocity;
        }
        else if (_isMovingVertically && (::abs(_prevVerticalStartY - getPosition().y) >= 24))
        {
            // if we are moving verically and the diff between our last
            // veritical movement is great enough, swap direction of x velocity
            if(_isMovingLeft) // start moving right
            {
                _isMovingLeft = false;
                _isMovingRight = true;
            } else { // start moving left
                _isMovingLeft = true;
                _isMovingRight = false;
            }
            // extremeley nasty, however bug would not go away.
            if(_handlingImpact && _isMovingLeft)
            {
                _isMovingLeft = false;
                _isMovingRight = true;
                _handlingImpact = false;
            }
            _velX = _isMovingRight ? _velocity : -_velocity;
            _velY = 0;
            _isMovingVertically = false;
            std::cout << "moving down and will moving: "<< _velX <<std::endl;
        }
        else
        {
            _velX = _velX;
        }
    }
}

void ECE_Centipede::_handleImpactTurn()
{
    _handlingImpact = true;
    _prevVerticalStartY = getPosition().y;
    _isMovingVertically = true;

    _velY = _isMovingDown ? _velocity : -_velocity;
    _velX = 0;
}



void ECE_Centipede::evaluateHeadPosition(std::vector<sf::Vector2f> mushroomPositions, float deltaTime)
{
    if (_isHead)
    {
        move(_velX * deltaTime, _velY * deltaTime);
    }
    _updateHeadVelocities(mushroomPositions, deltaTime);
}

// expected behavior of the body segments:
// the segments should move to the previous segment's position when the previous segments position
// is over the desired distance away.

float _distance(sf::Vector2f a, sf::Vector2f b)
{
    return std::sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

void ECE_Centipede::evaluateBodyPosition(sf::Vector2f prevSegmentPosition, float deltaTime)
{
    if (!_isHead && (_distance(getPosition(), prevSegmentPosition) > 24))
    {
        setPosition(prevSegmentPosition);
        // return true;
        // move(_velX * deltaTime, _velY * deltaTime);
    }
    else
    {
        // return false;
    }
}