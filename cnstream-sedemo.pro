QT += quick
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    qvideocapture.cpp \
    qvideocapturethread.cpp \
    worker/cnstreamworker.cpp \
    worker/detection_post_proc.cpp \
    worker/displayer.cpp \
    worker/mot_filter.cpp \
    worker/ssd_pre_proc.cpp \
    QPicProvider.cpp \
    worker/swdecoder.cpp \
    worker/fpscal.cpp

RESOURCES += qml.qrc \
    qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    main.qml.autosave \
    resources/images/car.jpg \
    resources/images/left.jpg \
    resources/images/right.jpg \
    resources/images/播放器背景.jpg \
    resources/images/1_u93.png \
    resources/images/0911-单兵2.1_16.png \
    resources/images/0911-单兵2.1_18.png \
    resources/images/0911-单兵2.1_20.png \
    resources/images/0911-单兵2.1_35.png \
    resources/images/0911-单兵2.1_41.png \
    resources/images/0911-单兵2.1_43.png \
    resources/images/close_disable.png \
    resources/images/close_down.png \
    resources/images/close_hover.png \
    resources/images/close_normal.png \
    resources/images/icon-退出全屏.png \
    resources/images/menu_down.png \
    resources/images/menu_hover.png \
    resources/images/menu_normal.png \
    resources/images/min_down.png \
    resources/images/min_hover.png \
    resources/images/min_normal.png \
    resources/images/player.png \
    resources/images/progress.png \
    resources/images/shadow.png \
    resources/images/u12.png \
    resources/images/u14.png \
    resources/images/u16.png \
    resources/images/u17.png \
    resources/images/u18.png \
    resources/images/u19.png \
    resources/images/u28.png \
    resources/images/u33.png \
    resources/images/u69.png \
    resources/images/u79.png \
    resources/images/u81.png \
    resources/images/u83.png \
    resources/images/u85.png \
    resources/images/u87.png \
    resources/images/u89.png \
    resources/images/u91.png \
    resources/images/u114.png \
    resources/images/关闭.png \
    resources/images/展开_u75.png \
    resources/images/快进.png \
    resources/images/快进 (1).png \
    resources/images/播放.png \
    resources/images/收起_u72.png \
    resources/images/暂停.png \
    resources/images/最大化 (1).png \
    resources/images/最小化.png \
    resources/images/查看.png \
    resources/images/视频结构化-车辆识别2_18.png \
    resources/images/视频结构化-车辆识别2_21.png \
    resources/images/ic_accessible_black_24px.svg

HEADERS += \
    qvideocapture.h \
    qvideocapturethread.h \
    worker/cnstreamworker.h \
    worker/detection_post_proc.h \
    worker/displayer.h \
    worker/mot_filter.h \
    worker/ssd_pre_proc.h \
    QPicProvider.h \
    worker/swdecoder.h \
    worker/fpscal.h

INCLUDEPATH += $$PWD/dependencies/include
LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lglog
LIBS += -L$$PWD/dependencies/lib/ -lcnstream -lcnrt -lcncodec -ldeepsort

