#ifndef __SPIDER_H__
#define __SPIDER_H__

#include <SFML/Graphics.hpp>
#include <list>
#include <MushroomManager.hpp>
class Spider

{
    public:
        struct SpiderState
        {
            bool isDead;
            unsigned int centerPosX;
            unsigned int centerPosY;
            sf::Texture texture;
            sf::Sprite sprite;
        };
        void resetSpider();
        sf::Sprite getSprite() {return _state.sprite; };
        explicit Spider(unsigned int spiderWindowWidth, unsigned int spiderWindowHeight);
        SpiderState getState(float deltaTime, std::list<MushroomManager::MushroomState>& mushroomStates);
    private:
        SpiderState _state;
        float _velocity = 300.0;
        unsigned int _windowWidth, _windowHeight; 
        float _velX;
        float _velY;
        float _elapsedTime;

};
#endif // __SPIDER_H__