#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//License: Public Domain
//Author: elf
//EMail: elf198012@gmail.com

#include <QMainWindow>

class IoThread;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(IoThread *pThread, QWidget *parent = 0);
    ~MainWindow();
private slots:
    void onIoThreadFinished();
    void on_leMessage_returnPressed();
    void onMessage(const QString &strMessage);
private:
    IoThread *_pThread;
    //QString _strMessages;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
