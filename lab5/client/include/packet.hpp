#ifndef __PACKET_H__
#define __PACKET_H__

struct ClientPacket
{
    bool client_exiting;
    char key_pressed;
};

struct ServerPacket
{
    bool server_exited;
};
#endif // __PACKET_H__