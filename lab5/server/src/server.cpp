#include <server.hpp>
#include <stdexcept>
#include <packet.hpp>
#include <cstring>
#include <iostream>

Server::Server(uint16_t port)
{
    if (_socket.bind(port) != sf::Socket::Done)
    {
        throw std::runtime_error( "error binding socket" );
    }
    _port = port;
    _clientPort = 0;
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
        _clientPort = recvdPort;
        ClientPacket clientData;
        memcpy(&clientData, _buffer, sizeof(ClientPacket));
        std::cout << clientData.key_pressed <<" "<< receivedAddr.toString() <<std::endl;
    }
}
void Server::sendExitNotification()
{
    ServerPacket serv_data = {true};
    memcpy(_buffer, &serv_data, sizeof(ServerPacket));
    if(_clientPort != 0)
    {
        std::cout <<"sending to client port " << _clientPort <<std::endl;
        _socket.send(_buffer, sizeof(ServerPacket), sf::IpAddress("127.0.0.1"), _clientPort);
    }
}