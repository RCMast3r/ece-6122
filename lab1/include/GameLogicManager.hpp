/*
Author: 
    Ben Hall
Class: 
    ECE6122
Last Date Modified: 
    9/30/24
Description:
    game logic manager class description header file
*/
#ifndef __GAMELOGICMANAGER_H__
#define __GAMELOGICMANAGER_H__

#include <optional>

#include <SFML/Graphics.hpp>

#include <StarShip.hpp>
#include <MushroomManager.hpp>
#include <ECE_LaserBlast.hpp>
#include <ECE_Centipede.hpp>

// user story:
// i want this class to manage all of the game-level logic (score, lives, when to switch screens, etc.)

// requirements:
// - "Use a std::list to maintain the state and location of the mushrooms."

class GameLogicManager
{
    public:
        GameLogicManager(bool &construction_failure);
        bool alive();
        void tickGame(float deltaTime);
    private:
        std::optional<StarShip::StarShipControlInput> _handleKeyPress(sf::Keyboard::Key keyPressCode);
        void _handleKeyRelease(sf::Keyboard::Key keyPressCode);
        void _drawGame();
    private:
        enum class GameState {
            STARTUP = 0,
            PLAYING = 1,
            NUM_STATES
        };
        std::vector<ECE_LaserBlast> _lasers;
        sf::Clock _clock; 
        StarShip::StarShipControlInput _gendInput = {};
        GameState _gameState;
        sf::Font _font;
        sf::Text _score;
        sf::Texture _startDisplay;
        sf::Sprite _startDisplaySprite;
        sf::RenderWindow _window;
        StarShip _starship;
        Spider _spider;
        MushroomManager _mushroomManager;
        ECE_Centipede _centipede;
        int _currentScore = 0;
        
};

#endif // __GAMELOGICMANAGER_H__