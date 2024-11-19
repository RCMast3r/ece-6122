#ifndef __PACKET_H__
#define __PACKET_H__

struct ClientPacket
{
    bool client_exiting;
    char key_pressed;
    char key_released;
};

struct ServerPacket
{
    bool serverExited;
};
#endif // __PACKET_H__