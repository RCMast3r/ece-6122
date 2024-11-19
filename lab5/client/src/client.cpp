#include <client.hpp>
#include <stdexcept>
#include <packet.hpp>
#include <cstring>

Client::Client(uint16_t port)
{
    _socket.setBlocking(false);
    _port = port;
}

void Client::poll()
{
    std::size_t receivedDataSize;
    sf::IpAddress receivedAddr;
    
    ClientPacket test_data;
    test_data.client_exiting = false;
    test_data.key_pressed = 'w';
    
    memcpy(_buffer, &test_data, sizeof(ClientPacket));
    
    _socket.send(_buffer, sizeof(ClientPacket), sf::IpAddress("127.0.0.1"), _port);
    // if(receivedDataSize > 0)
    // {
    //     ClientPacket clientData;
    //     memcpy(_buffer, &clientData, sizeof(ClientPacket));
    // }
}