#ifndef __SERVER_H__
#define __SERVER_H__

#include <SFML/Network.hpp>
#include <cstdint>

class Server
{
    public:
        Server(uint16_t port);
        void poll();
        void sendExitNotification();
    private:
        sf::UdpSocket _socket;
        uint16_t _port, _clientPort;
        char _buffer[100];
};

#endif // __SERVER_H__