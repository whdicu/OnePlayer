QT += core gui multimedia multimediawidgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RC_ICONS = icon.ico

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    hook.cpp \
    imagewidget.cpp \
    main.cpp \
    musicbutton.cpp \
    onlinehandler.cpp \
    onlinemusicbutton.cpp \
    settinghandler.cpp \
    widget.cpp

HEADERS += \
    basemusicbutton.h \
    hook.h \
    imagewidget.h \
    musicbutton.h \
    onlinehandler.h \
    onlinemusicbutton.h \
    settinghandler.h \
    widget.h

FORMS += \
    musicbutton.ui \
    onlinemusicbutton.ui \
    widget.ui

INCLUDEPATH += D:\coder\C++\HD

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
