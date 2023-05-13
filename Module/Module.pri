# 使用分设计的方式，将页面分成多个部分
FORMS += \
    $$PWD/controlbar.ui \
    $$PWD/sidebar.ui \
    $$PWD/titlebar.ui

HEADERS += \
    $$PWD/controlbar.h \        # 播放控制栏
    $$PWD/sidebar.h \           # 侧边栏
    $$PWD/slidervideo.h \       # 鼠标右键隐藏侧边栏和控制看
    $$PWD/titlebar.h            # 自定义标题栏

SOURCES += \
    $$PWD/controlbar.cpp \
    $$PWD/sidebar.cpp \
    $$PWD/slidervideo.cpp \
    $$PWD/titlebar.cpp
