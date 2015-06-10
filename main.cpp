//License: Public Domain
//Author: elf
//EMail: elf198012@gmail.com

#include "MainWindow.h"
#include <QApplication>
#include "IoThread.h"
#include <QThread>

int main(int argc, char *argv[])
{
    if(argc != 3)
        return 1;

    Qt::HANDLE h = QThread::currentThreadId();

    QApplication a(argc, argv);
    IoThread iotThread;
    iotThread.SetLocal(atoi(argv[1]));

    MainWindow window(&iotThread);
    window.show();

    iotThread.SetRemote("127.0.0.1", atoi(argv[2]));
    iotThread.Run();
    return a.exec();
}
