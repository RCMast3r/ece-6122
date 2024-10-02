/*
Author: Ben Hall
Class: ECE6122
Last Date Modified: 9/30/24
Description: common game utils
*/

#ifndef __CENTIPEDE_UTILS_H__
#define __CENTIPEDE_UTILS_H__
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

namespace utils
{
    float pointwiseDistance(sf::Vector2f a, sf::Vector2f b);
    sf::Vector2f spriteCenterPos(sf::Sprite spr);
};
#endif // __UTILS_H__