#---------------------------------------------------------------------------------------
# @功能：       ffmpeg读取视频图像模块，其它模块想要使用直接调用即可
# @编译器：     Desktop Qt 5.12.5 MSVC2017 64bit（也支持其它编译器）
# @Qt IDE：    D:/Qt/Qt5.12.5/Tools/QtCreator/share/qtcreator
#
# @开发者     mhf
# @邮箱       1603291350@qq.com
# @时间       2022-09-15 14:27:25
# @备注
#---------------------------------------------------------------------------------------

# 加载库，ffmpeg n5.1.2版本

LIBS += -LD:/Qt/ffmpeg/lib/ -lavcodec -lavfilter -lavformat -lswscale -lavutil -lswresample -lavdevice
INCLUDEPATH += D:/Qt/ffmpeg/include
DEPENDPATH += D:/Qt/ffmpeg/include


HEADERS += \
    $$PWD/readthread.h \
    $$PWD/videodecode.h \
    $$PWD/audiodecode.h

SOURCES += \
    $$PWD/readthread.cpp \
    $$PWD/videodecode.cpp \
    $$PWD/audiodecode.cpp
