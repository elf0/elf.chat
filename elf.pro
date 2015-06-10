#-------------------------------------------------
#
# Project created by QtCreator 2015-06-08T15:56:46
#
#-------------------------------------------------

QT       += core gui webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = elf
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        MainWindow.cpp \
    IoThread.cpp \
    Database.cpp

HEADERS  += MainWindow.h \
    IoThread.h \
    Packet.h \
    Type.h \
    DoubleNode.h \
    List.h \
    Database.h

FORMS    += MainWindow.ui

CONFIG(release, release|debug){
INCLUDEPATH += $$PWD/include
LIBS += -L$$PWD/lib -luv -lsqlite3 -ldl
}

CONFIG(debug, release|debug){
INCLUDEPATH += $$PWD/include
LIBS += -L$$PWD/debug -luv -lsqlite3 -ldl
}

DISTFILES += \
    chat.css \
    chat.html
