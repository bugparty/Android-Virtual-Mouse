//
// Created by fancy on 2021/5/30.
//

#ifndef SOCKETSERVER_PACKET_H
#define SOCKETSERVER_PACKET_H
#include <arpa/inet.h>
struct mouse_move {
    int x;
    int y;
};
struct mouse_click{
    int key_code;
    int status;
};
typedef int MsgType;
MsgType AUTH = 0x01;
MsgType MOVE = 0x02;
MsgType CLICK = 0x03;
struct Packet{
    MsgType type;
    union {
        struct mouse_move move;
        struct mouse_click click;
    };
    void toNet(){
        type = ntohl(type);
        move.x = ntohl(move.x);
        move.y = ntohl(move.y);
    };
    void fromNet(){
        type = htonl(type);
        move.x = htonl(move.x);
        move.y = htonl(move.y);
    };
};
#define BTN_MOUSE 0x110
#endif //SOCKETSERVER_PACKET_H
