QT += core
QT += network
QT += websockets

#INCLUDEPATH+=$$PWD

HEADERS += \
#    $$PWD/private/p_qrpc_thread_ping.h \
    $$PWD/qrpc.h \
    $$PWD/qrpc_const.h \
    $$PWD/qrpc_controller_annotation.h \
    $$PWD/qrpc_global.h \
    $$PWD/qrpc_last_error.h \
    $$PWD/qrpc_log.h \
    $$PWD/qrpc_macro.h \
    $$PWD/qrpc_startup.h \
    $$PWD/qrpc_types.h \

SOURCES += \
#    $$PWD/private/p_qrpc_thread_ping.cpp \
    $$PWD/qrpc_controller_annotation.cpp \
    $$PWD/qrpc_log.cpp \
    $$PWD/qrpc_last_error.cpp \
    $$PWD/qrpc_types.cpp

