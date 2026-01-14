QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QT += core gui sql widgets
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    contact.cxx \
    contactdialog.cpp \
    dbdata.cpp \
    filedata.cpp \
    main.cpp \
    telephone.cpp

HEADERS += \
    contact.h \
    contactdialog.h \
    dataload.h \
    dbdata.h \
    filedata.h \
    telephone.h

FORMS += \
    telephone.ui

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
