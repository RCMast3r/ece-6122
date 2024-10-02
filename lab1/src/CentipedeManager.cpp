#include <CentipedeManager.hpp>
#include <cmath>
#include <GameUtils.hpp>
#include <iostream>

CentipedeManager::CentipedeManager() : _head(true),
                                       _body(false)
{
    initSegments();
}

void CentipedeManager::initSegments()
{
    _segments.clear();
    _segments.push_back(_head);
    auto curPos = _head.getPosition();
    ECE_Centipede head(true);
    ECE_Centipede body(false);
    for (int i = 0; i < 11; i++)
    {
        curPos.x -= 24;
        _body.setPosition({curPos.x, curPos.y});
        _segments.push_back(_body);
    }

    for( auto & seg : _segments)
    {
        std::cout << seg.isHead()<<  " " ;
    }
    std::cout <<std::endl;
}

float _mag(sf::Vector2f v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}
sf::Vector2f CentipedeManager::_normalize(sf::Vector2f v)
{
    float mag = std::sqrt(v.x * v.x + v.y * v.y);
    if (mag == 0)
        return sf::Vector2f(0, 0);
    return sf::Vector2f(v.x / mag, v.y / mag);
}

void CentipedeManager::evaluateSegments(std::vector<sf::Vector2f> mushroomPositions, float deltaTime)
{
    // for( auto & seg : _segments)
    // {
    //     std::cout << seg.isHead()<<  " " ;
    // }
    // std::cout <<std::endl;
    auto prevPosition = _segments[0].getPosition();
    sf::Vector2f prev2Position;
    size_t currentBodyCounter = 0;
    for (int i = 0; i < _segments.size(); ++i)
    {
        if (_segments[i].isHead())
        {
            currentBodyCounter = 0;
            _segments[i].evaluateHeadPosition(mushroomPositions, deltaTime);
        }
        else
        {
            currentBodyCounter++;
            prevPosition = _segments[i - 1].getPosition();
            if (utils::pointwiseDistance(_segments[i].getPosition(), prevPosition) > 24)
            {
                auto mag = _mag({prevPosition.x - _segments[i].getPosition().x, prevPosition.y - _segments[i].getPosition().y});
                auto xUnit = (prevPosition.x - _segments[i].getPosition().x) / mag;
                auto yUnit = (prevPosition.y - _segments[i].getPosition().y) / mag;
                _segments[i].setVelocity({xUnit * _segments[i].getVelocityMag(), yUnit * _segments[i].getVelocityMag()});
                _segments[i].move({xUnit * _segments[i].getVelocityMag() * deltaTime, yUnit * _segments[i].getVelocityMag() * deltaTime});
            }
            else
            {
                auto curVel = _segments[i].getVelocity();
                _segments[i].move({curVel.x * deltaTime, curVel.y * deltaTime});
            }
        }
    }
}

void CentipedeManager::handleSegmentImpact(size_t impactIndex)
{
    if (!((_segments.size() - 1) > (impactIndex + 1)))
    {
        // _segments[(impactIndex + 1)].
    }
}

bool CentipedeManager::evaluateLaser(sf::Vector2f position, int &currentScore)
{
    bool impacted = false;
    std::vector<int> indecesToErase;
    int inde = 0;
    for (auto &centipedeMember : _segments)
    {
        inde++;
        auto centipedeSpriteCenterPos = utils::spriteCenterPos(*((sf::Sprite *)&centipedeMember));
        if (utils::pointwiseDistance(centipedeSpriteCenterPos, position) < 24)
        {
            impacted = true;
            if (centipedeMember.isHead())
            {
                std::cout << "ind" << inde << std::endl;

                currentScore += 100;
                indecesToErase.push_back(inde);
                std::cout << "hit head" << std::endl;
            }
            else
            {
                std::cout << "hit body" << std::endl;
                currentScore += 10;
                centipedeMember.promoteToHead();
            }
        }
    }

    for (const auto &indece : indecesToErase)
    {
        auto it = _segments.begin();
        std::advance(it, indece);
        _segments.erase(it);
    }
    return impacted;
}

void CentipedeManager::evaluateMushroomLocations(std::vector<sf::Vector2f> mushroomPositions)
{
    for (const auto &mushroomPos : mushroomPositions)
    {
        for (auto &centipedeMember : _segments)
        {

            if (centipedeMember.isHead())
            {
                auto centipedLoc = utils::spriteCenterPos(*(sf::Sprite *)&centipedeMember);
                if (utils::pointwiseDistance(centipedLoc, mushroomPos) < 12)
                {
                    centipedeMember.handleBump();
                }
            }
        }
    }
}

void CentipedeManager::evaluateHeadPositions()
{
    for (auto &centipedeMemberOuter : _segments)
    {

        if (centipedeMemberOuter.isHead())
        {
            for (auto &centipedeMemberInner : _segments)
            {
                if (centipedeMemberInner.isHead())
                {
                    auto centipedOuterLoc = utils::spriteCenterPos(*(sf::Sprite *)&centipedeMemberOuter);
                    auto centipedInnerLoc = utils::spriteCenterPos(*(sf::Sprite *)&centipedeMemberInner);
                    if (utils::pointwiseDistance(centipedOuterLoc, centipedInnerLoc) < 12)
                    {
                        centipedeMemberInner.handleBump();
                    }
                }
            }
        }
    }
}

void CentipedeManager::drawOnWindow(sf::RenderWindow &window)
{
    for (auto &centiped : _segments)
    {
        window.draw(*(sf::Sprite *)&centiped);
    }
}