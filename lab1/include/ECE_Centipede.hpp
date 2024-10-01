#ifndef __ECE_CENTIPEDE_H__
#define __ECE_CENTIPEDE_H__

#include <SFML/Graphics.hpp>

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
        bool isHead() {return _isHead; }
        sf::Vector2f getVelocity() {return {_velX, _velY}; }
        void setVelocity(sf::Vector2f vel) {_velX = vel.x; _velY = vel.y;}
    private:
        float _distance(sf::Vector2f a, sf::Vector2f b);
        void _updateHeadVelocities(std::vector<sf::Vector2f> mushroomPositions, float deltaTime);
    private:
        float _orientationDeg = 0;
        bool _isHead;
        bool _isMovingVertically;
        bool _isMovingDown, _isMovingRight;
        sf::Texture _headTexture, _bodyTexture;
        float _followDistance; 
        float _velX, _velY, _prevVerticalStartY;
        // pos _start_pos;

};

#endif // __ECE_CENTIPEDE_H__