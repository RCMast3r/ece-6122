/*
Author:
    Ben Hall
Class:
    ECE6122
Last Date Modified:
    9/30/24
Description:
    starship class implementation source. handles user input state, etc.
*/

#include <StarShip.hpp>
#include <iostream>

StarShip::StarShip(unsigned int starshipWindowWidth, unsigned int starshipWindowHeight, unsigned int heightOffset, float velocity, bool &construction_error, std::string asset_path)
{
    _state.liveCount = 3;
    if (!_state.texture.loadFromFile(asset_path))
    {
        construction_error = true;
    }
    // a little bit wacky, but ensures that the sprite's texture data is along side
    // it in the struct for the m_texture pointer
    // I HAVE BEEN HAVING A LOT OF ISSUES WITH TEXTURE POINTERS it has been making me sad :-(
    _state.sprite.setTexture(_state.texture);
    _windowWidth = starshipWindowWidth;
    _windowHeight = starshipWindowHeight;
    // starts at the bottom ish like real game
    _state.sprite.setPosition({(float)(_windowWidth / 2), 500.0f});
    _state.centerPosX = _state.sprite.getPosition().x + (float)(_state.texture.getSize().x / 2);
    _state.centerPosY = _state.sprite.getPosition().y - (float)(_state.texture.getSize().y / 2);
    construction_error = false;
}

void StarShip::command(StarShipControlInput input, float deltaTime, Spider::SpiderState spiderState, Spider &spider)
{
    // whole lotta hard-coded values i know, but im the only one working on the code-base
    float xMoveCmd = (float)(input.rightInput - input.leftInput) * 30.0;
    float yMoveCmd = (float)(input.downInput - input.upInput) * 30.0;
    if ((_state.sprite.getPosition().x + (xMoveCmd * deltaTime)) > ((_windowWidth - (_state.texture.getSize().x / 2))) ||
        (_state.sprite.getPosition().x + (xMoveCmd * deltaTime) < 0))
    {
        xMoveCmd = 0;
    }
    if ((_state.sprite.getPosition().y + (yMoveCmd * deltaTime)) > 520 ||
        (_state.sprite.getPosition().y + (yMoveCmd * deltaTime)) < 300)
    {
        yMoveCmd = 0;
    }

    if ((::abs(((int)spiderState.centerPosX) - ((int)_state.centerPosX)) < 20) && (::abs(((int)spiderState.centerPosY) - ((int)_state.centerPosY)) < 25))
    {
        _state.liveCount--;
        // reset the position of the starship and "kill" the spider
        _state.sprite.setPosition({(float)(_windowWidth / 2), 500.0f});
        spider.resetSpider();
    }
    else
    {
        _state.sprite.move({xMoveCmd * deltaTime, yMoveCmd * deltaTime});
        _state.emitLaser = input.isFiring;
    }
    if (_state.liveCount == 0)
    {
        _state.isDead = true;
    }
    else
    {
        _state.isDead = false;
    }
    _state.centerPosX = _state.sprite.getPosition().x + (float)(_state.texture.getSize().x / 2);
    _state.centerPosY = _state.sprite.getPosition().y - (float)(_state.texture.getSize().y / 2);
}
