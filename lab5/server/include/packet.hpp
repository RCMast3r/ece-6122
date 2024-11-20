/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 11/19/24
Description: shared packet description header
*/

#ifndef __PACKET_H__
#define __PACKET_H__

struct ClientPacket
{
    bool clientExiting;
    char keyPressed;
    char keyReleased;
};

struct ServerPacket
{
    bool serverExited;
};
#endif // __PACKET_H__