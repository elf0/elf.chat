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
MainWindow::MainWindow(IoThread *pThread, QWidget *parent)
    : QMainWindow(parent)
    , _pThread(pThread)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralWidget->setLayout(ui->verticalLayout);

    QString strPath = "file://" + QApplication::applicationDirPath();
    QUrl urlHtml = QUrl(strPath + "/chat.html");
    ui->wvMessages->load(urlHtml);

    connect(_pThread, &IoThread::message, this, &MainWindow::onMessage);
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
    QByteArray baMessage = strMessage.toUtf8();
    U32 nBytes = baMessage.size();

    if(!_pThread->PostMessage((const Char*)baMessage.data(), nBytes))
        return;

    QDateTime dtTime =  QDateTime::currentDateTime();
    QString strMessageTag = "<p class=\"MessageRight\"><span class=\"Time\">" + dtTime.toString("HH:mm:ss") + "</span><br/>" + strMessage + "</p>\n";
    QWebFrame *pwfFrame = ui->wvMessages->page()->mainFrame();
    QWebElement weDocument = pwfFrame->documentElement();
    QWebElement weMessages = weDocument.findFirst("#Messages");
    weMessages.appendInside(strMessageTag);
    pwfFrame->setScrollBarValue(Qt::Vertical, pwfFrame->scrollBarMaximum(Qt::Vertical));
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
