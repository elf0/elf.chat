#ifndef IOTHREAD_H
#define IOTHREAD_H

//License: Public Domain
//Author: elf
//EMail: elf198012@gmail.com

#include <uv.h>
#include <QObject>
#include "Type.h"
#include "List.h"

#define MAX_UDP_DATA_SIZE 65507

typedef enum{
    etPostUdp
}IoRequestType;

typedef struct{
    DoubleNode node;
    IoRequestType type;
    Byte *pData;
    U32 nBytes;
}IoRequest;

class IoThread : public QObject
{
    Q_OBJECT
public:
    IoThread(QObject *pParent = 0);
    ~IoThread();
    void SetLocal(U16 nPort);
    void SetRemote(const QString &strIp, U16 nPort);
    Bool PostUdp(Byte *pData, U16 nBytes);

    Bool Run();

signals:
    void receivedUdp(unsigned char *pData, unsigned short nBytes);
private:
    Bool Post(IoRequest *pRequest);
    inline void ProcessRequests();
    inline void ProcessRequest(IoRequest *pRequest);
    static void onRequest(uv_async_t* handle);
    List _requests;
    List _pendingRequests;
    uv_thread_t _thread;
    uv_mutex_t _mutex;
    uv_async_t _uvaRequestNofity;
    uv_udp_t _udp;
//    uv_udp_send_t _udpSendRequest;
    struct sockaddr_in _siRemote;
};

#endif // IOTHREAD_H
