#-------------------------------------------------
#
# Project created by QtCreator 2014-04-05T15:44:51
#
#-------------------------------------------------

QT       += core gui sql testlib
QMAKE_CXXFLAGS += -std=c++0x

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tab_opener
TEMPLATE = app
INCLUDEPATH += include

SOURCES += \
    src/main.cpp \
    src/hoverbutton.cpp \
    src/mainwindow.cpp \
    src/datastore.cpp \
    src/configwidget.cpp \
    src/charmbutton.cpp \
    src/presetbutton.cpp \
    src/util.cpp \
    src/dstable.cpp \
    src/test.cpp \
    src/dstablefav.cpp \
    src/noteswidget.cpp \
    src/filebrowser.cpp \
    src/commandwidget.cpp \
    src/cmdexecbutton.cpp \
    src/historywidget.cpp \
    src/historybutton.cpp

HEADERS  += \
    include/hoverbutton.h \
    include/mainwindow.h \
    include/datastore.h \
    include/configwidget.h \
    include/charmbutton.h \
    include/presetbutton.h \
    include/util.h \
    include/dstable.h \
    include/test.h \
    include/dstablefav.h \
    include/noteswidget.h \
    include/filebrowser.h \
    include/commandwidget.h \
    include/cmdexecbutton.h \
    include/historywidget.h \
    include/historybutton.h \
    include/history.h

FORMS    += \
    ui/mainwindow.ui \
    ui/configwidget.ui \
    ui/noteswidget.ui \
    ui/filebrowser.ui \
    ui/commandwidget.ui \
    ui/historywidget.ui

RESOURCES += \
    images.qrc

OTHER_FILES += \
    README.md \
    quicknotes.txt
