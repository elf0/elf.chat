//License: Public Domain
//Author: elf
//EMail: elf198012@gmail.com

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "IoThread.h"
#include <QWebFrame>
#include <QWebElement>
#include <QThread>
static uv_async_t async;
MainWindow::MainWindow(Database *pDb, IoThread *pThread, QWidget *parent)
    : QMainWindow(parent)
    , _pDb(pDb)
    , _pThread(pThread)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralWidget->setLayout(ui->verticalLayout);

    QString strPath = QApplication::applicationDirPath();
    QString strUrlPath = "file://" + strPath;
    QUrl urlHtml = QUrl(strUrlPath + "/chat.html");
    ui->wvMessages->load(urlHtml);

    connect(_pThread, &IoThread::receivedUdp, this, &MainWindow::onReceivedUdp);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onIoThreadFinished()
{
}

void MainWindow::on_leMessage_returnPressed(){
    Qt::HANDLE h = QThread::currentThreadId();
    QString strMessage = ui->leMessage->text();
    std::string str = strMessage.toStdString();
    U32 nBytes = str.size();
    if(nBytes == 0)
        return;

    U64 nId = _pDb->Find(str);
    if(nId == 0){
        if(!PostMessage((const Char*)str.data(), nBytes))
            return;
    }
    else{
        if(!PostIndexedMessage(nId))
            return;
    }

    QDateTime dtTime =  QDateTime::currentDateTime();
    QString strMessageTag = "<p class=\"MessageRight\"><span class=\"Time\">" + dtTime.toString("HH:mm:ss") + "</span><br/>" + strMessage + "</p>\n";
    QWebFrame *pwfFrame = ui->wvMessages->page()->mainFrame();
    QWebElement weDocument = pwfFrame->documentElement();
    QWebElement weMessages = weDocument.findFirst("#Messages");
    weMessages.appendInside(strMessageTag);
    pwfFrame->setScrollBarValue(Qt::Vertical, pwfFrame->scrollBarMaximum(Qt::Vertical));
}

Bool MainWindow::PostHello(){
    U32 nPacketSize = sizeof(Packet_Hello);
    Packet_Hello *pPacket = (Packet_Hello*)malloc(nPacketSize);
    pPacket->size = nPacketSize;
    pPacket->type = ptHello;
    pPacket->bEncrypted = 0;
    pPacket->protocolVersion = ELF_PROTOCOL_VERSION;
    pPacket->indexVersion = 0;
    return _pThread->PostUdp((Byte*)pPacket, nPacketSize);
}

Bool MainWindow::PostMessage(const Char *pMessage, U16 nBytes){
    Qt::HANDLE h = QThread::currentThreadId();
    //    if(nBytes > MAX_MESSAGE_SIZE)
    //        return false;

    U32 nPacketSize = sizeof(Packet) + nBytes;
    Packet_Message *pPacket = (Packet_Message*)malloc(nPacketSize);
    pPacket->size = nPacketSize;
    pPacket->type = ptMessage;
    memcpy(pPacket->szMessage, pMessage, nBytes);
    return _pThread->PostUdp((Byte*)pPacket, nPacketSize);
}

Bool MainWindow::PostIndexedMessage(U16 nIndex){
    U32 nPacketSize = sizeof(Packet_IndexedMessage);
    Packet_IndexedMessage *pPacket = (Packet_IndexedMessage*)malloc(nPacketSize);
    pPacket->size = nPacketSize;
    pPacket->type = ptIndexedMessage;
    pPacket->index = nIndex;
    return _pThread->PostUdp((Byte*)pPacket, nPacketSize);
}

void MainWindow::onReceivedUdp(Byte *pData, U16 nBytes){
    Packet *pPacket = (Packet*)pData;
    if(pPacket->size != nBytes)
        fprintf(stderr, "Error: bad packet!");
    else
        onPacket(pPacket);
    free(pData);
}

void MainWindow::onMessage(const QString &strMessage)
{
    Qt::HANDLE h = QThread::currentThreadId();
    QDateTime dtTime =  QDateTime::currentDateTime();
    QString strMessageTag = "<p class=\"MessageLeft\"><span class=\"Time\">" + dtTime.toString("HH:mm:ss") + "</span><br/>" + strMessage + "</p>\n";
    QWebFrame *pwfFrame = ui->wvMessages->page()->mainFrame();
    QWebElement weDocument = pwfFrame->documentElement();
    QWebElement weMessages = weDocument.findFirst("#Messages");
    weMessages.appendInside(strMessageTag);
    pwfFrame->setScrollBarValue(Qt::Vertical, pwfFrame->scrollBarMaximum(Qt::Vertical));
}

Bool MainWindow::onPacket(Packet *pPacket){
    switch(pPacket->type){
    default:
        return false;
    case ptMessage:{
        Packet_Message *pMessage = (Packet_Message*)pPacket;
        onMessage(QString::fromUtf8((const char*)pMessage->szMessage, pMessage->size - sizeof(Packet)));
    }break;
    case ptIndexedMessage:{
        Packet_IndexedMessage *pMessage = (Packet_IndexedMessage*)pPacket;
        std::string strMessage;
        if(_pDb->FindId(pMessage->index, strMessage))
            onMessage(QString::fromStdString(strMessage));
    }break;
    }
    return true;
}
