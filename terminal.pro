greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += widgets serialport
    QT       += network
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}

TARGET = terminal
TEMPLATE = app

INCLUDEPATH += C:\npcap-sdk-1.05\Include
LIBS += C:\npcap-sdk-1.05\Lib\wpcap.lib

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    protocol_modbus.cpp \
    settingsdialog.cpp \
    protocol_hnz.cpp \
    protocol_iec101_104.cpp \
    protocol_DNP3.cpp \
    protocol_iecgoose.cpp \
    protocol_iec103.cpp

HEADERS += \
    mainwindow.h \
    protocol_modbus.h \
    settingsdialog.h \
    protocol_hnz.h \
    protocol.h \
    protocol_iec101_104.h \
    protocol_DNP3.h \
    protocol_iecgoose.h \
    protocol_iec103.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    terminal.qrc

DISTFILES += \
    myRc.rc
RC_FILE=\
    myRc.rc

