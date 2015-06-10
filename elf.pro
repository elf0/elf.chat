#-------------------------------------------------
#
# Project created by QtCreator 2015-06-08T15:56:46
#
#-------------------------------------------------

QT       += core gui webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = elf
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    IoThread.cpp

HEADERS  += MainWindow.h \
    IoThread.h \
    Packet.h \
    Type.h \
    DoubleNode.h \
    List.h

FORMS    += MainWindow.ui

CONFIG(release, release|debug){
INCLUDEPATH += $$PWD/include
LIBS += $$PWD/lib/libuv.a
#LIBS += $$PWD/lib/libsqlite3.a
}

CONFIG(debug, release|debug){
INCLUDEPATH += $$PWD/include
LIBS += $$PWD/debug/libuv.a
#LIBS += $$PWD/debug/libsqlite3.a
}

DISTFILES += \
    chat.css \
    chat.html
