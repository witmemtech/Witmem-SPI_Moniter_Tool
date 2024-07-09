QT       += core gui printsupport
QT       += serialport

VERSION = 0.0.9
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
TARGET = SPI_Moniter

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ArrayLegedLabel.cpp \
    ArraySettingsDialog.cpp \
    AvrCalculator.cpp \
    ChangnelSelectShowWidget.cpp \
    DefineData.cpp \
    FrequencyDomainSettingDialog.cpp \
    FrequencyToolsMainWindow.cpp \
    LightPoint.cpp \
    RubberScrollArea.cpp \
    SerialWork.cpp \
    ThreadSaveData.cpp \
    ThreadSaveWavData.cpp \
    TimeDomainSettingDialog.cpp \
    main.cpp \
    qcustomplot.cpp

HEADERS += \
    ArrayLegedLabel.h \
    ArraySettingsDialog.h \
    AvrCalculator.h \
    ChangnelSelectShowWidget.h \
    DefineData.h \
    FrequencyDomainSettingDialog.h \
    FrequencyToolsMainWindow.h \
    LightPoint.h \
    RubberScrollArea.h \
    SerialWork.h \
    ThreadSaveData.h \
    ThreadSaveWavData.h \
    TimeDomainSettingDialog.h \
    qcustomplot.h

FORMS += \
    ArrayLegedLabel.ui \
    ArraySettingsDialog.ui \
    ChangnelSelectShowWidget.ui \
    FrequencyDomainSettingDialog.ui \
    FrequencyToolsMainWindow.ui \
    TimeDomainSettingDialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/x86/ -lUSB2XXX
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/x86/ -lUSB2XXX
else:unix: LIBS += -L$$PWD/x86/ -lUSB2XXX

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

RESOURCES += \
    ResourceFile.qrc
