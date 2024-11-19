#ifndef __CLIENT_H__
#define __CLIENT_H__
#include <cstdint>
#include <SFML/Network.hpp>
#include <optional>

class Client
{
    public:
        Client(uint16_t port);
        void poll(std::optional<char> keyPressed, bool isExiting);
    private:
        sf::UdpSocket _socket;
        uint16_t _port;
        char _buffer[100];
};
#endif // __CLIENT_H__