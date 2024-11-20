/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 11/19/24
Description: client class for wrapping the socket and packing / unpacking from the data buffer
*/
#ifndef __CLIENT_H__
#define __CLIENT_H__
#include <cstdint>
#include <SFML/Network.hpp>
#include <optional>

class Client
{
    public:
        /// @brief constructor
        /// @param port port to use
        Client(uint16_t port);
        /// @brief poll client function for server interaction / sending commands and getting status
        /// @param keyPressed potentially empty key press to send
        /// @param isExiting send is exiting flag
        /// @return false if the server has exited, true if still good
        bool poll(std::optional<char> keyPressed, bool isExiting);
    private:
        sf::UdpSocket _socket;
        uint16_t _port;
        char _buffer[100];
};
#endif // __CLIENT_H__