#include <Spider.hpp>
#include <iostream>
#include <random>
Spider::Spider(unsigned int spiderWindowWidth, unsigned int spiderWindowHeight)
{
    _state.texture.loadFromFile("graphics/spider.png");
    _state.sprite.setTexture(_state.texture);
    _windowWidth = spiderWindowWidth;
    _windowHeight = spiderWindowHeight;
    // starts at the bottom ish like real game
    _state.sprite.setPosition( { 75, 300 });
    _velX = _velocity; // always start out going right
    _velY = _velocity; // always start out going right
    _elapsedTime = 0;
}

void Spider::resetSpider()
{
    std::random_device rd;  
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> xRand(0, 1);
    auto randXStart = (xRand(gen)==1) ? 800.0f : 75.0f;
    _state.sprite.setPosition( { (float)randXStart , 300.0f });
}
Spider::SpiderState Spider::getState(float deltaTime, std::list<MushroomManager::MushroomState>& mushroomStates)
{
    std::random_device rd;  
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> xRand(0, 2);
    std::uniform_int_distribution<int> yRand(0, 2);

    // every 1.5 seconds, switch directions randomly
    _elapsedTime += deltaTime;
    if( _elapsedTime > 0.7)
    {
        _velX = _velocity * (xRand(gen) -1);
        // _velY = _velocity * (yRand(gen) -1);
        _elapsedTime = 0;
    }
    // next we will check to ensure that the spider is only moving within the desired area

    if( (_state.sprite.getPosition().x + (_velX * deltaTime)) > ((_windowWidth - (_state.texture.getSize().x / 2) ) ) 
        ||
        (_state.sprite.getPosition().x + (_velX * deltaTime) < 0))
    {
        _velX*=-1;
    }
    if( (_state.sprite.getPosition().y + (_velY * deltaTime) ) > 520 
    ||
        (_state.sprite.getPosition().y + (_velY * deltaTime)) < 300 )
    {
        _velY*=-1;
    }
    _state.sprite.move({ (_velX * deltaTime), (_velY*deltaTime)});

    auto posx = _state.sprite.getPosition().x + (_state.texture.getSize().x /2);
    auto posy = _state.sprite.getPosition().y - (_state.texture.getSize().y /2);
    
    int ind = 0;
    std::vector<int> toErase;
    for(auto & mushroom : mushroomStates)
    {
        if( (::abs(posx - (mushroom.x + (27/2))) < (50)) && (::abs(posy - (mushroom.y + (27/2))) < 26))
        {
            toErase.push_back(ind);
        }
        ind++;
    }
    for(auto & indToImpact : toErase)
    {
        auto it = mushroomStates.begin();
        std::advance(it, indToImpact);
        mushroomStates.erase(it);
    }

    _state.centerPosX = posx;
    _state.centerPosY = posy;
    return _state;
}

