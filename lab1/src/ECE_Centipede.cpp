#include <ECE_Centipede.hpp>
#include <iostream>
// constructor for the linked list that creates all of the nodes, setting all
ECE_Centipede::ECE_Centipede(bool isHead) : _isHead(isHead)
{
    _headTexture.loadFromFile("graphics/CentipedeHead.png");
    _bodyTexture.loadFromFile("graphics/CentipedeBody.png");
    if (isHead)
    {
        _velX = 200.0;
        _velY = 0.0;
        for (int i = 0; i < 11; i++)
        {
            std::cout << "appending new node in constructor" << std::endl;
            std::shared_ptr<ECE_Centipede> newNode = std::make_shared<ECE_Centipede>(false);
            this->append(newNode);
        }
        setTexture(_headTexture);
    }
    else
    {
        setTexture(_bodyTexture);
        _velX = 0.0;
        _velY = 0.0;
    }
    setPosition({(float)(1036 / 2), (float)53});
}

// if this section is a head, update its own position and make all positions of every other non-heads
// an "offset" from the parent's node

// - [ ] make
// motion:
// the head starts moving right to left, starting from the top-middle of the screen
// each body piece follows it, and a head hits a mushroom or wall it turns down and
// then starts going the opposite direction it was. Once the head has reached the bottom,
// instead of turning down once at an each or mushroom, it starts progressing back up
// to the top of the screen.

void ECE_Centipede::_updateHeadVelocities(std::vector<sf::Vector2f> mushroomPositions)
{
    if(getPosition().x)
}

void ECE_Centipede::evaluate(std::vector<sf::Vector2f> mushroomPositions, float deltaTime)
{
    auto currentPosition = getPosition();

    std::cout << "moving head" << std::endl;
    
    move(_velX * deltaTime, _velY * deltaTime);
    auto newHeadPos = getPosition();

    if (_next != nullptr)
    {
        _next->follow(newHeadPos);
    }
}
void ECE_Centipede::follow(sf::Vector2f newPos)
{
    auto currentPos = getPosition();

    setPosition({newPos.x - 27, newPos.y});

    if (_next != nullptr)
    {
        _next->follow(currentPos);
    }
}

void ECE_Centipede::setBodyPosition(sf::Vector2f position)
{

    setPosition(position);
}

void ECE_Centipede::append(std::shared_ptr<ECE_Centipede> newNode)
{
    if (!_next)
    {
        _next = newNode;
    }
    else
    {
        _next->append(newNode);
    }
}
