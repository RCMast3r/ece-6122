/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 11/19/24
Description: server class description header. also contains descriptions of the received commands that we can receive.
*/

#ifndef __SERVER_H__
#define __SERVER_H__

#include <SFML/Network.hpp>
#include <cstdint>
#include <optional>
class Server
{
    public:
        enum class Direction
        {
            UP,
            DOWN,
            LEFT,
            RIGHT,
            NONE
        };
        enum class SpeedChange
        {
            INC,
            DEC,
            NONE
        };
        struct ReceivedCommand
        {
            Direction dirEnum;
            SpeedChange velChange;
            bool disappear = false;
        };
        Server(uint16_t port);
        std::optional<ReceivedCommand> poll();
        void sendExitNotification();
    private:
        sf::UdpSocket _socket;
        uint16_t _port, _clientPort;
        char _buffer[100];
};

#endif // __SERVER_H__