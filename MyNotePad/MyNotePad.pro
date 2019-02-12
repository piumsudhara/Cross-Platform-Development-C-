QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyNotePad
TEMPLATE = app


SOURCES += main.cpp\
        MyNotePad.cpp \
    finddialog.cpp

HEADERS  += MyNotePad.h \
    finddialog.h

FORMS    += MyNotePad.ui \
    finddialog.ui

RESOURCES += \
    icons.qrc
