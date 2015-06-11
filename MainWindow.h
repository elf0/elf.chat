#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//License: Public Domain
//Author: elf
//EMail: elf198012@gmail.com

#include <QMainWindow>
#include "Database.h"
#include "Packet.h"

class IoThread;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Database *pDb, IoThread *pThread, QWidget *parent = 0);
    ~MainWindow();
private slots:
    void onIoThreadFinished();
    void on_leMessage_returnPressed();
    void onReceivedUdp(Byte *pData, U16 nBytes);
    void onMessage(const QString &strMessage);
private:
    Bool PostHello();
    Bool PostMessage(const Char *pMessage, U16 nSize);
    Bool PostIndexedMessage(U16 nIndex);
    Bool onPacket(Packet *pPacket);
    Database *_pDb;
    IoThread *_pThread;
    //QString _strMessages;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
