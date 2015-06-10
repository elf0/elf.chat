//License: Public Domain
//Author: elf
//EMail: elf198012@gmail.com

#include <stdio.h>
//#include <stddef.h>

#include <QThread>
#include "IoThread.h"

#define MAX_UDP_DATA_SIZE 65507
#define MAX_MESSAGE_SIZE (MAX_UDP_DATA_SIZE - sizeof(Packet))

static void ThreadFuntion(void* arg);
static void Udp_onAllocate(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
static void Udp_onReceived(uv_udp_t* handle, ssize_t nread, const uv_buf_t* pBuf,
                           const struct sockaddr* addr, unsigned flags);
static void IoThread_onSent(uv_udp_send_t* req, int status);

IoThread::IoThread(QObject *pParent)
    : QObject(pParent)
{
    List_Initialize(&_events);
    uv_mutex_init(&_mutex);
    uv_async_init(uv_default_loop(), &_event, onEvent);
    _event.data = this;

    uv_udp_init(uv_default_loop(), &_udp);
    _udp.data = this;
}

IoThread::~IoThread()
{
}

void IoThread::onEvent(uv_async_t* handle){
    Qt::HANDLE h = QThread::currentThreadId();
    IoThread *pThread = (IoThread*)handle->data;
    pThread->ProcessEvents();
}

void IoThread::SetLocal(U16 nPort)
{
    struct sockaddr_in addr;
    uv_ip4_addr("127.0.0.1", nPort, &addr);
    uv_udp_bind(&_udp, (const struct sockaddr*) &addr, 0);
    uv_udp_recv_start(&_udp, Udp_onAllocate, Udp_onReceived);
}

void IoThread::SetRemote(const QString &strIp, U16 nPort){
    uv_ip4_addr(strIp.toUtf8().data(), nPort, &_siRemote);
}

Bool IoThread::Post(Event *pEvent){
    uv_mutex_lock(&_mutex);
    List_Push(&_events, (DoubleNode*)pEvent);
    uv_mutex_unlock(&_mutex);
    return uv_async_send(&_event) == 0;
}

void IoThread::ProcessEvents(){
    List *pEvents = &_events;
    uv_mutex_lock(&_mutex);
    while(List_NotEmpty(pEvents)){
        Event *pEvent = (Event*)List_Pop(pEvents);
        ProcessEvent(pEvent);
    }
    uv_mutex_unlock(&_mutex);
}

void IoThread::ProcessEvent(Event *pEvent){
    switch(pEvent->type){
    case etPostMessage:{
        uv_udp_send_t *pRequest = (uv_udp_send_t*)(pEvent + 1);
        Packet_Message *pPacket = (Packet_Message*)(pRequest + 1);
        uv_buf_t buf = uv_buf_init((char*)pPacket, pPacket->size);
        int nResult = uv_udp_send(pRequest, &_udp, &buf, 1, (const struct sockaddr*)&_siRemote, IoThread_onSent);
    }break;
    case etPostIndexedMessage:
        break;
    }
}

Bool IoThread::PostMessage(const Char *pMessage, U16 nBytes){
    Qt::HANDLE h = QThread::currentThreadId();
    if(nBytes > MAX_MESSAGE_SIZE)
        return false;

    U32 nPacketSize = sizeof(Packet) + nBytes;
    Event *pEvent = (Event*)malloc(sizeof(Event) + sizeof(uv_udp_send_t) + nPacketSize);
    pEvent->type = etPostMessage;

    uv_udp_send_t *pRequest = (uv_udp_send_t*)(pEvent + 1);
    Packet_Message *pPacket = (Packet_Message*)(pRequest + 1);
    pPacket->size = nPacketSize;
    pPacket->type = ptMessage;
    memcpy(pPacket->szMessage, pMessage, nBytes);
    return Post(pEvent);
}

Bool IoThread::Run(){
    return uv_thread_create(&_thread, ThreadFuntion, this) == 0;
}

static void Udp_onAllocate(uv_handle_t* handle,
                           size_t suggested_size,
                           uv_buf_t* buf) {
    char *slab = (char*)malloc(65536);
    buf->base = slab;
    buf->len = 65536;
}

static void Udp_onReceived(uv_udp_t* handle,
                           ssize_t nread,
                           const uv_buf_t* pBuf,
                           const struct sockaddr* addr,
                           unsigned flags){
    Qt::HANDLE h = QThread::currentThreadId();
    if(nread == 0){
        free(pBuf->base);
        return;
    }

    if (nread < 0) {
        free(pBuf->base);
        return;
    }

    IoThread *pThread = (IoThread*)handle->data;
    Packet *pPacket = (Packet*)pBuf->base;
    if(pPacket->size != nread)
        fprintf(stderr, "Error: bad packet!");
    else
        IoThread::onPacket(pThread, pPacket);
    free(pBuf->base);
}

static void IoThread_onSent(uv_udp_send_t* req, int status){
    int r;
    if(status != 0){
        printf("Client_onSent\n");
    }

    Event *pEvent = (Event*)req - 1;
    free(pEvent);
}

static const char * g_szMessages[] = {
    "Hello!" , "Hi!", "Where are you?"
};

Bool IoThread::onPacket(IoThread *pThread, Packet *pPacket){
    switch(pPacket->type){
    default:
        return false;
    case ptMessage:{
        Packet_Message *pMessage = (Packet_Message*)pPacket;
        emit pThread->message(QString::fromUtf8((const char*)pMessage->szMessage, pMessage->size - sizeof(Packet)));
    }break;
    case ptIndexedMessage:{
        Packet_IndexedMessage *pMessage = (Packet_IndexedMessage*)pPacket;
    }break;
    }
    return true;
}

//Bool IoThread::PostIndexedMessage(struct sockaddr_in *pAddress, U16 nIndex){
//    U32 nPacketSize = sizeof(Packet_IndexedMessage);
//    Packet_IndexedMessage *pPacket = (Packet_IndexedMessage*)malloc(nPacketSize);
//    pPacket->size = nPacketSize;
//    pPacket->type = ptIndexedMessage;
//    pPacket->index = nIndex;

//    uv_buf_t buf = uv_buf_init((char*)pPacket, pPacket->size);
//    return 0 == uv_udp_send(&_udpSendRequest, &_udp,
//                            &buf, 1, (const struct sockaddr*)pAddress,
//                            IoThread_onSent);
//}

static void ThreadFuntion(void* arg){
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
