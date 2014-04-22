#-------------------------------------------------
#
# Project created by QtCreator 2014-04-05T15:44:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tab_opener
TEMPLATE = app
INCLUDEPATH += include

SOURCES += \
    src/main.cpp \
    src/hoverbutton.cpp \
    src/filelist.cpp \
    src/cmdlist.cpp \
    src/mainwindow.cpp

HEADERS  += \
    include/hoverbutton.h \
    include/filelist.h \
    include/cmdlist.h \
    include/mainwindow.h

FORMS    += \
    ui/filelist.ui \
    ui/cmdlist.ui \
    ui/mainwindow.ui

RESOURCES += \
    images.qrc
