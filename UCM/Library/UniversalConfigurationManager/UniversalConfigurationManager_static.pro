#-------------------------------------------------
#
# Project created by QtCreator 2016-12-14T14:32:07
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UniversalConfigurationManager
TEMPLATE = app
CONFIG += static

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt. 
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    Ucm_UniversalConfigurationManager.cpp \
    CommunicationLib/Ucm_CommunicationLib.cpp \
    CoreConfig/Ucm_CoreConfig.cpp \
    ClkClockTab/Ucm_ClkClockTab.cpp \
    ClkSgTab/Ucm_ClkSgTab.cpp \
    ClkTsTab/Ucm_ClkTsTab.cpp \
    AdvancedTab/Ucm_AdvancedTab.cpp \
    RtcMasterTab/Ucm_RtcMasterTab.cpp \
    PpsMasterTab/Ucm_PpsMasterTab.cpp \
    PpsSlaveTab/Ucm_PpsSlaveTab.cpp \
    IrigSlaveTab/Ucm_IrigSlaveTab.cpp \
    IrigMasterTab/Ucm_IrigMasterTab.cpp \
    TodSlaveTab/Ucm_TodSlaveTab.cpp \
    RedHsrPrpTab/Ucm_RedHsrPrpTab.cpp \
    PtpHcTab/Ucm_PtpHcTab.cpp \
    PtpOcTab/Ucm_PtpOcTab.cpp \
    PtpTcTab/Ucm_PtpTcTab.cpp \
    ConfigTab/Ucm_ConfigTab.cpp

HEADERS  += Ucm_UniversalConfigurationManager.h \
    CommunicationLib/Ucm_CommunicationLib.h \
    CoreConfig/Ucm_CoreConfig.h \
    ClkClockTab/Ucm_ClkClockTab.h \
    ClkSgTab/Ucm_ClkSgTab.h \
    ClkTsTab/Ucm_ClkTsTab.h \
    AdvancedTab/Ucm_AdvancedTab.h \
    RtcMasterTab/Ucm_RtcMasterTab.h \
    PpsMasterTab/Ucm_PpsMasterTab.h \
    PpsSlaveTab/Ucm_PpsSlaveTab.h \
    IrigSlaveTab/Ucm_IrigSlaveTab.h \
    IrigMasterTab/Ucm_IrigMasterTab.h \
    TodSlaveTab/Ucm_TodSlaveTab.h \
    RedHsrPrpTab/Ucm_RedHsrPrpTab.h \
    PtpHcTab/Ucm_PtpHcTab.h \
    PtpOcTab/Ucm_PtpOcTab.h \
    PtpTcTab/Ucm_PtpTcTab.h \
    ConfigTab/Ucm_ConfigTab.h \
    DebugStream/Ucm_DebugStream.h

FORMS    += Ucm_UniversalConfigurationManager.ui \
    ClkClockTab/Ucm_ClkClockTab.ui \
    ClkSgTab/Ucm_ClkSgTab.ui \
    ClkTsTab/Ucm_ClkTsTab.ui \
    AdvancedTab/Ucm_AdvancedTab.ui \
    RtcMasterTab/Ucm_RtcMasterTab.ui \
    PpsMasterTab/Ucm_PpsMasterTab.ui \
    PpsSlaveTab/Ucm_PpsSlaveTab.ui \
    IrigSlaveTab/Ucm_IrigSlaveTab.ui \
    IrigMasterTab/Ucm_IrigMasterTab.ui \
    TodSlaveTab/Ucm_TodSlaveTab.ui \
    RedHsrPrpTab/Ucm_RedHsrPrpTab.ui \
    PtpHcTab/Ucm_PtpHcTab.ui \
    PtpOcTab/Ucm_PtpOcTab.ui \
    PtpTcTab/Ucm_PtpTcTab.ui \
    ConfigTab/Ucm_ConfigTab.ui

RC_FILE = Ucm_UniversalConfigurationManager.rc

DISTFILES += \
    Ucm_UniversalConfigurationManager.ico \
    Ucm_UniversalConfigurationManager.rc
