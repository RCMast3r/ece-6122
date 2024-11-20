/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 11/19/24
Description: server class implementation file.  
*/


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

std::optional<Server::ReceivedCommand> Server::poll()
{
    std::size_t receivedDataSize;
    sf::IpAddress receivedAddr;
    uint16_t recvdPort;
    _socket.receive(_buffer, 100, receivedDataSize, receivedAddr, recvdPort);

    if(receivedDataSize > 0 )
    {
        Server::ReceivedCommand cmdIn = { Server::Direction::NONE, Server::SpeedChange::NONE, false };

        _clientPort = recvdPort;
        ClientPacket clientData;
        memcpy(&clientData, _buffer, sizeof(ClientPacket));
        std::cout << clientData.keyPressed <<" "<< receivedAddr.toString() <<std::endl;

        if(clientData.keyPressed == 'w')
        {
            cmdIn.dirEnum = Server::Direction::UP;
            return cmdIn;
        } else if(clientData.keyPressed == 'a')
        {
            cmdIn.dirEnum = Server::Direction::LEFT;
            return cmdIn;
        } else if(clientData.keyPressed == 's')
        {
            cmdIn.dirEnum = Server::Direction::DOWN;
            return cmdIn;
        } else if(clientData.keyPressed == 'd')
        {
            cmdIn.dirEnum = Server::Direction::RIGHT;
            return cmdIn;
        } else if(clientData.keyPressed == 'g')
        {
            cmdIn.velChange = Server::SpeedChange::INC;
            return cmdIn;
        } else if(clientData.keyPressed == 'h')
        {
            cmdIn.velChange = Server::SpeedChange::DEC;
            return cmdIn;
        } else if(clientData.keyPressed == 'q' || clientData.keyPressed == 'x')
        {
            cmdIn.disappear = true;
            return cmdIn;
        }

    }

    return std::nullopt;
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