//License: Public Domain
//Author: elf
//EMail: elf198012@gmail.com

#include <QApplication>
#include <QThread>
#include "MainWindow.h"
#include "Database.h"
#include "IoThread.h"

int main(int argc, char *argv[])
{
    if(argc != 3)
        return 1;

    Qt::HANDLE h = QThread::currentThreadId();
    QApplication a(argc, argv);

    Database database;
    QString strPath = QApplication::applicationDirPath();
    if(!database.Open(strPath.toStdString() + "/system.db"))
        return 2;

    IoThread iotThread;
    iotThread.SetLocal(atoi(argv[1]));

    MainWindow window(&database, &iotThread);
    window.show();

    iotThread.SetRemote("127.0.0.1", atoi(argv[2]));
    iotThread.Run();
    return a.exec();
}
