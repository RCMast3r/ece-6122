#ifndef __ECE_CENTIPEDE_H__
#define __ECE_CENTIPEDE_H__

#include <SFML/Graphics.hpp>

#include <memory>
// linked list made from the sprite base class
// responsibility: detect collision with entities: (laser positions and starship position)
class ECE_Centipede : sf::Sprite
{
    public:
        ECE_Centipede(bool isHead);
        void setBodyPosition(sf::Vector2f position);
        void append(std::shared_ptr<ECE_Centipede> newNode);
        void evaluate(std::vector<sf::Vector2f> mushroomPositions, float deltaTime);
        bool isHead() {return _isHead; }
        void follow(sf::Vector2f newPos);
        std::shared_ptr<ECE_Centipede> _next = nullptr;
    private:
        void _updateHeadVelocities(std::vector<sf::Vector2f> mushroomPositions);
    private:
        bool _isHead;
        sf::Texture _headTexture, _bodyTexture;
        
        float _velX, _velY;
        // pos _start_pos;

};

#endif // __ECE_CENTIPEDE_H__