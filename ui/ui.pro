QT       += core gui
QT       += gui
QT       += opengl
QT       += charts
QT       += sql
QT       += serialport
QT       += axcontainer
QT       += widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ui
TEMPLATE = app

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32-g++ {
    LIBS += -lopengl32
}

SOURCES += \
    batchproc.cpp \
    calculate.cpp \
    color_set.cpp \
    com_set.cpp \
    commonfunction.cpp \
    figure.cpp \
    figureresult.cpp \
    filesdeal.cpp \
    global.cpp \
    insdata.cpp \
    instest.cpp \
    main.cpp \
    mainwindow.cpp \
    openglwidget.cpp \
    paintonline.cpp \
    playback.cpp \
    playback_readdata.cpp \
    qcustomplot.cpp \
    region.cpp \
    region_selection.cpp \
    related.cpp \
    running_set.cpp \
    startprocessing.cpp \
    startsavingtoharddisk.cpp \
    system_set.cpp \
    waveview.cpp

HEADERS += \
    batchproc.h \
    calculate.h \
    color_set.h \
    com_set.h \
    commonfunction.h \
    figure.h \
    figureresult.h \
    filesdeal.h \
    global.h \
    insdata.h \
    instest.h \
    mainwindow.h \
    openglwidget.h \
    paintonline.h \
    playback.h \
    playback_readdata.h \
    qcustomplot.h \
    region.h \
    region_selection.h \
    related.h \
    running_set.h \
    startprocessing.h \
    startsavingtoharddisk.h \
    system_set.h \
    waveview.h

FORMS += \
    batchproc.ui \
    color_set.ui \
    com_set.ui \
    figureresult.ui \
    instest.ui \
    mainwindow.ui \
    openglwidget.ui \
    playback.ui \
    region.ui \
    region_selection.ui \
    related.ui \
    running_set.ui \
    system_set.ui \
    waveview.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += -fopenmp

QMAKE_CXXFLAGS += -fopenmp
#个人笔记本安装路径
INCLUDEPATH += C:\winpcap\WpdPack_4_1_2\WpdPack\Include
LIBS += C:\winpcap\WpdPack_4_1_2\WpdPack\Lib\x64\Packet.lib
LIBS += C:\winpcap\WpdPack_4_1_2\WpdPack\Lib\x64\wpcap.lib
LIBS += C:\winpcap\WpdPack_4_1_2\WpdPack\Lib\Packet.lib
LIBS += C:\winpcap\WpdPack_4_1_2\WpdPack\Lib\wpcap.lib

RESOURCES += \
    styleqrc.qrc

DISTFILES +=
