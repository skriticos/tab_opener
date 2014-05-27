#-------------------------------------------------
#
# Project created by QtCreator 2014-04-05T15:44:51
#
#-------------------------------------------------

QT       += core widgets sql testlib
CONFIG   += c++11

TARGET = tab_opener
TEMPLATE = app

INCLUDEPATH += include

SOURCES += \
    src/buttons/charmbutton.cpp \
    src/buttons/cmdexecbutton.cpp \
    src/buttons/historybutton.cpp \
    src/buttons/hoverbutton.cpp \
    src/buttons/presetbutton.cpp \
    src/widgets/commandwidget.cpp \
    src/widgets/configwidget.cpp \
    src/widgets/filebrowserwidget.cpp \
    src/widgets/historywidget.cpp \
    src/widgets/mainwindow.cpp \
    src/widgets/noteswidget.cpp \
    src/datastore.cpp \
    src/dstable.cpp \
    src/dstablefav.cpp \
    src/main.cpp \
    src/test.cpp \
    src/util.cpp

HEADERS  += \
    include/charmbutton.h \
    include/cmdexecbutton.h \
    include/commandwidget.h \
    include/configwidget.h \
    include/datastore.h \
    include/dstable.h \
    include/dstablefav.h \
    include/filebrowserwidget.h \
    include/history.h \
    include/historybutton.h \
    include/historywidget.h \
    include/hoverbutton.h \
    include/mainwindow.h \
    include/noteswidget.h \
    include/presetbutton.h \
    include/test.h \
    include/util.h

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
