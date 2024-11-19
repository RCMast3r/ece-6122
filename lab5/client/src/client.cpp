#include <client.hpp>
#include <stdexcept>
#include <packet.hpp>
#include <cstring>
#include <iostream>

Client::Client(uint16_t port)
{
    if (_socket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
        throw std::runtime_error( "error binding socket" );
    }
    _socket.setBlocking(false);
    _port = port;
}

void Client::poll(std::optional<char> keyPressed, bool isExiting)
{
    
    if(keyPressed)
    {
        ClientPacket data = {false, *keyPressed};
        memcpy(_buffer, &data, sizeof(ClientPacket));
        _socket.send(_buffer, sizeof(ClientPacket), sf::IpAddress("127.0.0.1"), _port);
    } else if(isExiting)
    {
        ClientPacket data = {true, 'x'};
        memcpy(_buffer, &data, sizeof(ClientPacket));
        _socket.send(_buffer, sizeof(ClientPacket), sf::IpAddress("127.0.0.1"), _port);
    }
    
    std::size_t receivedDataSize;
    sf::IpAddress receivedAddr;
    uint16_t recvdPort;
    _socket.receive(_buffer, 100, receivedDataSize, receivedAddr, recvdPort);
    if(receivedDataSize > 0 )
    {
        ServerPacket serverData;
        memcpy(&serverData, _buffer, sizeof(ServerPacket));
        std::cout << serverData.serverExited <<" "<< receivedAddr.toString() <<std::endl;
    }
}