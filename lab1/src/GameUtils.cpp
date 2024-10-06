/*
Author: Ben Hall
Class: ECE6122
Last Date Modified: 9/30/24
Description: cross-component utilities
*/

#include <GameUtils.hpp>

#include <cmath>
namespace utils
{
    float pointwiseDistance(sf::Vector2f a, sf::Vector2f b)
    {
        return std::sqrt( (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
    }

    sf::Vector2f spriteCenterPos(sf::Sprite spr)
    {
        auto textureSize = spr.getTexture()->getSize();
        auto xOffset = textureSize.x / 2;
        auto yOffset = textureSize.y / 2;

        auto curPos = spr.getPosition();
        return {curPos.x + xOffset, curPos.y + yOffset};
    }
};