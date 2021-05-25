QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
    FFMPEG_HOME = D:/ffmpeg-4.3.2
    # 设置头文件的目录，以便编译器能够找到头文件
#    INCLUDEPATH += D:/ffmpeg-4.3.2/include
#    # 设置导入库的目录和需要链接的导入库
#    LIBS += -LD:\ffmpeg-4.3.2\lib \
#            -lavcodec \
#            -lavdevice \
#            -lavfilter \
#            -lavformat \
#            -lavutil \
#            -lpostproc \
#            -lswscale \
#            -lswresample

    # 设置头文件的目录，以便编译器能够找到头文件
    INCLUDEPATH += $${FFMPEG_HOME}/include
    # 设置导入库的目录和需要链接的导入库
    LIBS += -L$${FFMPEG_HOME}/lib \
            -lavcodec \
            -lavdevice \
            -lavfilter \
            -lavformat \
            -lavutil \
            -lpostproc \
            -lswscale \
            -lswresample


message($${FFMPEG_HOME});
}

macx {
    FFMPEGHOM_mac = /usr/local/Cellar/ffmpeg/4.4_1
    INCLUDEPATH += $${FFMPEGHOM_mac}/include
    LIBS += -L$${FFMPEGHOM_mac}/lib \
            -lavcodec \
            -lavdevice \
            -lavfilter \
            -lavformat \
            -lavutil \
            -lpostproc \
            -lswscale \
            -lswresample \
            -lavresample

message($${FFMPEGHOM_mac});
}

