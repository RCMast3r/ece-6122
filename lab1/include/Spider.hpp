/*
Author: Ben Hall
Class: ECE6122
Last Date Modified: 9/30/24
Description: Spider class definition that handles all calculations and interactions related to the spider in the window
*/

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
        /// @brief reset the spider. gets called upon death
        void resetSpider();
        /// @brief gets the sprite to draw
        /// @return copy of the sprite
        sf::Sprite getSprite() {return _state.sprite; };
        /// @brief explicit constructor for the spider class with args for the window that it should stay within. 
        //          (these are actually hard coded right now and dont do anything)
        /// @param spiderWindowWidth 
        /// @param spiderWindowHeight 
        explicit Spider(unsigned int spiderWindowWidth, unsigned int spiderWindowHeight);

        /// @brief gets the current state derived from the time delta. mushroom states get updated here as well
        /// @param deltaTime frame time in sec
        /// @param mushroomStates states of the mushroom
        /// @return current spider state
        SpiderState getState(float deltaTime, std::list<MushroomManager::MushroomState>& mushroomStates);
        
    private:
        SpiderState _state;
        float _velocity = 100.0;
        unsigned int _windowWidth, _windowHeight; 
        float _velX;
        float _velY;
        float _elapsedTime;

};
#endif // __SPIDER_H__