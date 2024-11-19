#include <server.hpp>
#include <stdexcept>
#include <packet.hpp>
#include <cstring>
#include <iostream>

Server::Server(uint16_t port)
{
    if (_socket.bind(port) != sf::Socket::Done)
    {
        throw std::runtime_error( "error binding to socket" );
    }
    _port = port;
    _socket.setBlocking(false);
}

void Server::poll()
{
    std::size_t receivedDataSize;
    sf::IpAddress receivedAddr;
    uint16_t recvdPort;
    _socket.receive(_buffer, 100, receivedDataSize, receivedAddr, recvdPort);
    if(receivedDataSize > 0 )
    {
        ClientPacket clientData;
        memcpy(&clientData, _buffer, sizeof(ClientPacket));
        std::cout << clientData.key_pressed <<" "<< receivedAddr.toString() <<std::endl;
    }
}