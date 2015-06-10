#ifndef PACKET_H
#define PACKET_H

//License: Public Domain
//Author: elf
//EMail: elf198012@gmail.com

typedef enum{
    ptMessage,
    ptIndexedMessage
}PacketType;

#define PACKET_HEADER \
    U16 size;\
    U16 type

typedef struct{
    PACKET_HEADER;
}Packet;

typedef struct{
    PACKET_HEADER;
    U16 index;
}Packet_IndexedMessage;

typedef struct{
    PACKET_HEADER;
    Char szMessage[1];
}Packet_Message;

typedef struct{
    PACKET_HEADER;
    U16 nAdjustSize;
}Packet_AdjustPacketSize;

#endif // PACKET_H

