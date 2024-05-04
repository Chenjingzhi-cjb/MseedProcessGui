QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    mainwindow/ \
    mseed_process/include/ \
    pyenv_configurator/include/

SOURCES += \
    mainwindow/mainwindow.cpp \
    main.cpp

HEADERS += \
    mainwindow/mainwindow.h \
    mseed_process/include/mseed_process.hpp \
    pyenv_configurator/include/pyenv_configurator.hpp

FORMS += \
    mainwindow/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

RC_FILE += app.rc
