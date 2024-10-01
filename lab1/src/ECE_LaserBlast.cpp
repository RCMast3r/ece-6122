#include <ECE_LaserBlast.hpp>
#include <cmath>
#include <iostream>
ECE_LaserBlast::BlastState ECE_LaserBlast::getState(float deltaTime, std::list<MushroomManager::MushroomState>& mushroomStates, std::vector<sf::Vector2f> centipedePositions, int &currentScore)
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
            std::cout <<"impacting" <<std::endl;
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

    return _state;
}