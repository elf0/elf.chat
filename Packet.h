#ifndef PACKET_H
#define PACKET_H

//License: Public Domain
//Author: elf
//EMail: elf198012@gmail.com

#define ELF_PROTOCOL_VERSION 0

typedef enum{
    ptProxy,
    ptHello,
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
    U32 nIp;
    U16 nPort;
    U8 nProxies;
    U8 szAlign[5];
}Packet_Proxy;

typedef struct{
    PACKET_HEADER;
    U16 protocolVersion;
    U16 indexVersion;//0: no index database. other: read version from file.
    U32 bEncrypted: 1;//0: not encrypted. 1: encrypted.
    U64 nNodeId;//Remember me.
}Packet_Hello;

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

