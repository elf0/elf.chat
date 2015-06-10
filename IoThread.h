#ifndef IOTHREAD_H
#define IOTHREAD_H

//License: Public Domain
//Author: elf
//EMail: elf198012@gmail.com

#include <uv.h>
#include <QObject>
#include "Type.h"
#include "List.h"
#include "Packet.h"

typedef enum{
    etPostMessage, etPostIndexedMessage
}EventType;

typedef struct{
    DoubleNode node;
    EventType type;
}Event;

class IoThread : public QObject
{
    Q_OBJECT
public:
    IoThread(QObject *pParent = 0);
    ~IoThread();
    void SetLocal(U16 nPort);
    void SetRemote(const QString &strIp, U16 nPort);
    static inline Bool onPacket(IoThread *pThread, Packet *pPacket);
    Bool PostMessage(const Char *pMessage, U16 nSize);
    Bool Run();

signals:
    void message(const QString &strMessage);
private:
    Bool Post(Event *pEvent);
    inline void ProcessEvents();
    inline void ProcessEvent(Event *pEvent);
    static void onEvent(uv_async_t* handle);
    //inline Bool PostIndexedMessage(struct sockaddr_in *pAddress, U16 nIndex);
    List _events;
    uv_thread_t _thread;
    uv_mutex_t _mutex;
    uv_async_t _event;
    uv_udp_t _udp;
//    uv_udp_send_t _udpSendRequest;
    struct sockaddr_in _siRemote;
};

#endif // IOTHREAD_H
