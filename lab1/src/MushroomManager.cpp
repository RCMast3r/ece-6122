#include <MushroomManager.hpp>

#include <random>

MushroomManager::MushroomManager(unsigned int mushroomAreaWidth, unsigned int mushroomAreaHeight, size_t numMushrooms, bool &construction_error)
{
    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    // divide the areas by 27 to get a grid location in height / width. these will be the centers of the 
    std::uniform_int_distribution<unsigned int> yDistribution(3, (mushroomAreaHeight / 27) -1);
    std::uniform_int_distribution<unsigned int> xDistribution(3, (mushroomAreaWidth / 27) -1 );

    // TODO make sure that none of the mushrooms are located on top of one another
    for(size_t n = 0; n < numMushrooms; n++)
    {
        auto x = xDistribution(gen);
        auto y = yDistribution(gen);
        _states.push_back( {x * 27, y * 27, 0} );
    }
    if ((!_commonFirstStageTexture.loadFromFile("graphics/Mushroom0.png")) || !_commonSecondStageTexture.loadFromFile("graphics/Mushroom1.png"))
    {
       construction_error = true; 
    }

    construction_error = false;

}

sf::Sprite MushroomManager::makeMushroomGraphic(MushroomManager::MushroomState state)
{
    if(state.state == 1) {
        sf::Sprite mushroomSprite(_commonSecondStageTexture);
        mushroomSprite.setPosition(state.x, state.y);
        return mushroomSprite;
    } 
    // fallthrough else {
    sf::Sprite mushroomSprite(_commonFirstStageTexture);
    mushroomSprite.setPosition(state.x, state.y);
    return mushroomSprite;
    // }
}

