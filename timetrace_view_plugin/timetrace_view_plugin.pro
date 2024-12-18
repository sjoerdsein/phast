QT += core gui
QT += widgets

TARGET = timetrace_view_plugin
TEMPLATE = lib

QMAKE_CXXFLAGS += -Wno-unused-parameter
win32:LIBS += -L$$OUT_PWD/../phast_gui -lphast_gui


# Add qwt directory to search path
win32:INCLUDEPATH += $$[QT_INSTALL_PREFIX]"/include/qwt-qt5"
unix:INCLUDEPATH += $$[QT_INSTALL_PREFIX]"/include/qwt"

# Also the qwt lib
win32:LIBS += -lqwt-qt5
unix:LIBS += -lqwt
unix:DEFINES += TT_USE_BASE_QWT

# We also require libtimetag
LIBS += -llibtimetag

INCLUDEPATH += $$PWD/..

DEFINES += TIMETRACE_VIEW_PLUGIN_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        timetrace_view_plugin.cpp \
    create_trace_ui_dialog.cpp \
    plot_timetrace_ui.cpp \
    timetrace_calc.cpp

HEADERS += \
        timetrace_view_plugin.h \
        timetrace_view_plugin_global.h \ 
    create_trace_ui_dialog.h \
    plot_timetrace_ui.h \
    timetrace_calc.h

FORMS += \
        create_trace_ui_dialog.ui

unix {
    target.path = /usr/lib
    INSTALLS += target
}
