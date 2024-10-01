#ifndef __ECE_CENTIPEDE_H__
#define __ECE_CENTIPEDE_H__

#include <SFML/Graphics.hpp>

#include <memory>
// linked list made from the sprite base class
// responsibility: detect collision with entities: (laser positions and starship position)
class ECE_Centipede : sf::Sprite
{
    public:
        ECE_Centipede(float velocity, bool isHead);
        
        void append();
        void evaluate();
    private:
        bool _isHead;
        std::shared_ptr<ECE_Centipede> _next;
        float _velocity;
        pos _start_pos;

};

#endif // __ECE_CENTIPEDE_H__