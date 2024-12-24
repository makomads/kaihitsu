QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
win32:LIBS += -luser32

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    application.cpp \
    comboboxdialog.cpp \
    composition.cpp \
    configdialog.cpp \
    editorengine.cpp \
    editorview.cpp \
    filehistory.cpp \
    finddialog.cpp \
    mainwindow.cpp \
    model.cpp \
    versiondialog.cpp


HEADERS += \
    application.h \
    comboboxdialog.h \
    composition.h \
    configdialog.h \
    editorengine.h \
    editorview.h \
    filehistory.h \
    finddialog.h \
    mainwindow.h \
    model.h \
    version.h \
    versiondialog.h

FORMS += \
    comboboxdialog.ui \
    configdialog.ui \
    finddialog.ui \
    mainwindow.ui \
    versiondialog.ui

TRANSLATIONS += \
    kaihitsu_ja_JP.ts \
    kaihitsu_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    kaihitsu_en_US.ts

RESOURCES += \
    resource.qrc
