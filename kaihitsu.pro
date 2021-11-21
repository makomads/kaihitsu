#-------------------------------------------------
#
# Project created by QtCreator 2015-06-30T22:38:43
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kaihitsu
TEMPLATE = app
#QMAKE_LFLAGS = -static-libgcc
#QMAKE_LFLAGS += -static -static-libgcc
#QMAKE_CXXFLAGS += -Wno-sign-compare -Wno-unused-variable
#CFLAGS += -Wno-sign-compare
RC_FILE = winresource.rc
#QMAKE_CXXFLAGS += -std=c++11

win32:LIBS += -luser32

SOURCES +=\
    application.cpp \
    composition.cpp \
    editorengine.cpp \
    mainwindow.cpp \
    model.cpp \
    editorview.cpp \
    configdialog.cpp \
    finddialog.cpp \
    comboboxdialog.cpp \
    versiondialog.cpp \
    filehistory.cpp

HEADERS  +=\
    application.h \
    composition.h \
    editorengine.h \
    mainwindow.h \
    model.h \
    editorview.h \
    configdialog.h \
    finddialog.h \
    comboboxdialog.h \
    versiondialog.h \
    filehistory.h \
    version.h \
    winresource.rc

FORMS    += \
    mainwindow.ui \
    configdialog.ui \
    finddialog.ui \
    comboboxdialog.ui \
    versiondialog.ui


TRANSLATIONS+=  \
    translation_ja.ts \
    translation_en.ts

OTHER_FILES += \
    translation_en.ts \
    translation_ja.ts

RESOURCES += \
    resource.qrc
