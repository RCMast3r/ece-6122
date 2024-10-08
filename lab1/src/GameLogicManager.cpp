/*
Author: Ben Hall
Class: ECE6122
Last Date Modified: 9/30/24
Description: main game logic handling. holds and handles the interaction between the game window and the entities within it
*/

#include <GameLogicManager.hpp>
#include <iostream>
#include <string> 
// member initialize list that calls the constructor of the game window with the stuff we want
// the window size is currently hard-coded based off of the start screen size
GameLogicManager::GameLogicManager(bool &construction_failure) : _window(sf::VideoMode(1036, 570), "Centipede: the game"),
                                                                 _gameState(GameLogicManager::GameState::STARTUP),
                                                                 _starship(1036, 260, 310, 0.0, construction_failure),
                                                                 _mushroomManager(1036, 30, 30, construction_failure),
                                                                 _spider(1036, 260)
{

    _font.loadFromFile("graphics/arial.ttf");
    _lifeTexture.loadFromFile("graphics/StarShip.png");

    _score.setFont(_font);
    _score.setString(std::to_string(_currentScore));
    _score.setCharacterSize(30); 
    _score.setFillColor(sf::Color::White); 
    _score.setPosition((1036/2), 15);

    initLifeSprites();
    

    if (!_startDisplay.loadFromFile("graphics/StartupScreenBackGround.png"))
    {
        construction_failure = true;
        return;
    }

    _startDisplaySprite.setTexture(_startDisplay);
    construction_failure = false;
    
};

void GameLogicManager::initLifeSprites()
{
    sf::Sprite lifeSprite;
    lifeSprite.setTexture(_lifeTexture);

    float initXPos = 700;

    for(int i =0; i< 3; i++)
    {
        lifeSprite.setPosition({initXPos, 20});
        _livesSprites.push_back(lifeSprite);
        initXPos += lifeSprite.getTexture()->getSize().x + 1;
    }
}

bool GameLogicManager::alive()
{
    return _window.isOpen();
}

std::optional<StarShip::StarShipControlInput> GameLogicManager::_handleKeyPress(sf::Keyboard::Key keyPressCode)
{
    int desiredVelocity = 10; // TODO units

    if ((keyPressCode == sf::Keyboard::Enter) &&
        (_gameState == GameState::STARTUP))
    {
        _gameState = GameState::PLAYING;
        _gendInput = {};
        _starship.resetDeath();
        _currentScore = 0;
        return std::nullopt;
    }

    else if ((keyPressCode == sf::Keyboard::Left) &&
             (_gameState == GameState::PLAYING))
    {
        _gendInput.leftInput = desiredVelocity;
    }

    else if ((keyPressCode == sf::Keyboard::Right) &&
             (_gameState == GameState::PLAYING))
    {
        _gendInput.rightInput = desiredVelocity;
    }

    else if ((keyPressCode == sf::Keyboard::Up) &&
             (_gameState == GameState::PLAYING))
    {
        _gendInput.upInput = desiredVelocity;
    }

    else if ((keyPressCode == sf::Keyboard::Down) &&
             (_gameState == GameState::PLAYING))
    {
        _gendInput.downInput = desiredVelocity;
    }
    else if ((keyPressCode == sf::Keyboard::Space) &&
             (_gameState == GameState::PLAYING))
    {
        // return std::nullopt;
        ECE_LaserBlast blast(_starship.getCurrentState().centerPosX, _starship.getCurrentState().centerPosY, 400.0);
        _lasers.push_back(blast);
    }
    // by default return the generated input, this can be
    return _gendInput;
}

void GameLogicManager::_handleKeyRelease(sf::Keyboard::Key keyCode)
{
    if ((keyCode == sf::Keyboard::Left) &&
        (_gameState == GameState::PLAYING))
    {
        _gendInput.leftInput = 0;
    }

    else if ((keyCode == sf::Keyboard::Right) &&
             (_gameState == GameState::PLAYING))
    {
        _gendInput.rightInput = 0;
    }

    else if ((keyCode == sf::Keyboard::Up) &&
             (_gameState == GameState::PLAYING))
    {
        _gendInput.upInput = 0;
    }

    else if ((keyCode == sf::Keyboard::Down) &&
             (_gameState == GameState::PLAYING))
    {
        _gendInput.downInput = 0;
    }
}


void GameLogicManager::_drawGame()
{
    // Clear screen
    _window.clear();
    if (_gameState == GameState::STARTUP)
    {
        
        _window.draw(_startDisplaySprite);
    }
    else // _gameState == GameState::PLAYING for now
    {
        _score.setString(std::to_string(_currentScore));
        _window.draw(_score);
        auto starshipState = _starship.getCurrentState();
        _livesSprites.resize(starshipState.liveCount);
        for(const auto & life : _livesSprites)
        {
            _window.draw(life);
        }
        
        if (starshipState.isDead || _centipede.isDead()) // dead meaning all lives lost
        {
            _spider.resetSpider();
            _mushroomManager.resetAndGenerateMushrooms();
            _centipede.initSegments();
            initLifeSprites();
            _gameState = GameState::STARTUP;
        }
        else
        {
            _window.draw(starshipState.sprite);
            _window.draw(_spider.getSprite());
            auto mushroomStates = _mushroomManager.getMushroomStates();

            for (const auto &state : mushroomStates)
            {
                _window.draw(_mushroomManager.makeMushroomGraphic(state));
            }

            for (const auto &laser : _lasers)
            {
                _window.draw(*((sf::Sprite *)&laser));
            }

            
            // dealing with texture not popping up, memory bug 
            _centipede.drawOnWindow(_window);
        }
    }
    _window.display();
}
void GameLogicManager::tickGame(float deltaTime)
{
    sf::Event event;
    while (_window.pollEvent(event))
    {
        // Close window: exit
        switch (event.type)
        {
        case sf::Event::Closed:
        {
            _window.close();
            break;
        }
        case sf::Event::KeyPressed:
        {
            auto optInput = _handleKeyPress(event.key.code);
            if (optInput)
            {
                _gendInput = *optInput;
            }
            break;
        }
        case sf::Event::KeyReleased:
        {
            _handleKeyRelease(event.key.code);
            break;
        }
        default:
            continue;
        }
    }
    size_t ind = 0;
    auto & mushroomStates = _mushroomManager.getMushroomStates();
    for (auto &laser : _lasers)
    {
        auto state = laser.getState(deltaTime, mushroomStates, _centipede, _spider, _currentScore);
        if (state.impacted)
        {
            _lasers.erase(_lasers.begin() + ind);
        }
        ind++;
    }
    if (_gameState == GameState::PLAYING)
    {
        auto spiderState = _spider.getState(deltaTime, mushroomStates);
        _starship.command(_gendInput, deltaTime, spiderState, _spider);
        _centipede.evaluateSegments({}, deltaTime);
        

        std::vector<sf::Vector2f> mushroomPositions;
        for(const auto & state : mushroomStates)
        {
            
            mushroomPositions.push_back({(float)(state.x + 13), (float)(state.y+13)}); // fixed mushroom center pos
        }
        _centipede.evaluateMushroomLocations(mushroomPositions);
        
    }
    
    _drawGame();
}