#include <ECE_LaserBlast.hpp>
#include <CentipedeManager.hpp>
#include <cmath>
#include <iostream>
#include <GameUtils.hpp>
ECE_LaserBlast::BlastState ECE_LaserBlast::getState(float deltaTime, std::list<MushroomManager::MushroomState>& mushroomStates, CentipedeManager& centipede, Spider& spider, int &currentScore)
{
    move({0, -(_vel *deltaTime)} );

    if(getPosition().y < 0) {
        _state.impacted = true;
        return _state;
    } else {
        _state.impacted = false;
    }
    int ind = 0;
    std::vector<int> toErase;
    for(auto & mushroom : mushroomStates)
    {
        if( (::abs(getPosition().x - (mushroom.x + (27/2))) < (20)) && (::abs(getPosition().y - (mushroom.y + (27/2))) < 20))
        {
            std::cout << "impacted" <<std::endl;
            _state.impacted = true;
            toErase.push_back(ind);
        }
        ind++;
    }
    for(auto & indToImpact : toErase)
    {
        auto it = mushroomStates.begin();
        std::advance(it, indToImpact);
        if((*it).state == 1) {
            currentScore += 4;
            mushroomStates.erase(it);
        } else {
            (*it).state = 1;
        }
    }

    auto spiderSpriteCenterPos = utils::spriteCenterPos(spider.getSprite());
    if(utils::pointwiseDistance(spiderSpriteCenterPos, getPosition()) < 30)
    {
        _state.impacted = true;
        spider.resetSpider();
        currentScore += 300;
    }

    if(centipede.evaluateLaser(getPosition(), currentScore))
    {
        _state.impacted = true;
    }
    
    return _state;
}