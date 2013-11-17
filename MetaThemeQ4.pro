#-------------------------------------------------
#
# Project created by QtCreator 2013-11-17T15:56:13
#
#-------------------------------------------------

QT       += core gui

TARGET = MetaThemeQ4
TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$[QT_INSTALL_PLUGINS]/styles

SOURCES += metathemeplugin.cpp \
    qmetathemestyle.cpp \
    metathemeQt.cpp \
    metathemehelper.cpp

HEADERS += metathemeplugin.h \
    qmetathemestyle.h \
    metathemeQt.h \
    metathemehelper.h
OTHER_FILES += MetaThemeQ4.json

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

unix:!macx: LIBS += -lmetatheme -lQt3Support


unix:!macx:

INCLUDEPATH += /usr/include/metatheme
DEPENDPATH += /usr/include/metatheme
