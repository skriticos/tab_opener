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
    src/filelist.cpp \
    src/cmdlist.cpp \
    src/mainwindow.cpp \
    src/datastore.cpp \
    src/configwidget.cpp \
    src/charmbutton.cpp \
    src/presetbutton.cpp \
	src/filebutton.cpp \
    src/commandbutton.cpp \
    src/util.cpp \
    src/dstable.cpp \
    src/test.cpp \
    src/dstablefav.cpp

HEADERS  += \
    include/hoverbutton.h \
    include/filelist.h \
    include/cmdlist.h \
    include/mainwindow.h \
    include/datastore.h \
    include/configwidget.h \
    include/charmbutton.h \
    include/presetbutton.h \
	include/filebutton.h \
    include/commandbutton.h \
    include/util.h \
    include/dstable.h \
    include/test.h \
    include/dstablefav.h

FORMS    += \
    ui/filelist.ui \
    ui/cmdlist.ui \
    ui/mainwindow.ui \
    ui/configwidget.ui

RESOURCES += \
    images.qrc

OTHER_FILES += \
    README.md \
    quicknotes.txt
