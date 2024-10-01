#include <ECE_Centipede.hpp>

// constructor for the linked list that creates all of the nodes, setting all
ECE_Centipede::ECE_Centipede(bool isHead) : _isHead(isHead)
{
    if (isHead)
    {
        for (int i = 0; i < 11; i++)
        {
            std::shared_ptr<ECE_centipede> newNode = std::make_shared<ECE_centipede>(false);
            this->append(newNode);
        }
    }
}

// if this section is a head, update its own position and make all positions of every other non-heads 
// an "offset" from the parent's node

// motion:
// the head starts moving right to left, starting from the top-middle of the screen
// each body piece follows it, and a head hits a mushroom or wall it turns down and 
// then starts going the opposite direction it was. Once the head has reached the bottom, 
// instead of turning down once at an each or mushroom, it starts progressing back up 
// to the top of the screen.
void ECE_Centipede::evaluate(std::vector<sf::Vector2f> mushroomPositions, float deltaTime)
{
    for 
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

