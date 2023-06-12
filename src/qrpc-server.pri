QT += core

#INCLUDEPATH+=$$PWD

##declare to include CONFIG+=Q_RPC_WEBSOCKET
CONFIG(Q_RPC_WEBSOCKET){
QT += network
QT += websockets
HEADERS += \
    $$PWD/private/p_qrpc_listen_wss.h

SOURCES += \
    $$PWD/private/p_qrpc_listen_wss.cpp
}

##declare to include CONFIG+=Q_RPC_AMQP
CONFIG(Q_RPC_AMQP){
QT += network
HEADERS += \
    $$PWD/private/p_qrpc_listen_broker_amqp.h

SOURCES += \
    $$PWD/private/p_qrpc_listen_broker_amqp.cpp
}

##declare to include CONFIG+=Q_RPC_DATABASE
CONFIG(Q_RPC_DATABASE){
QT += network
QT += sql
HEADERS += \
    $$PWD/private/p_qrpc_listen_broker_database.h

SOURCES += \
    $$PWD/private/p_qrpc_listen_broker_database.cpp
}

##declare to include CONFIG+=Q_RPC_KAFKA
CONFIG(Q_RPC_KAFKA){
QT += network
HEADERS += \
    $$PWD/private/p_qrpc_listen_broker_kafka.h

SOURCES += \
    $$PWD/private/p_qrpc_listen_broker_kafka.cpp
}

##declare to include CONFIG+=Q_RPC_MQTT
CONFIG(Q_RPC_MQTT){
HEADERS += \
    $$PWD/private/p_qrpc_listen_broker_mqtt.h

SOURCES += \
    $$PWD/private/p_qrpc_listen_broker_mqtt.cpp
}

##declare to include CONFIG+=Q_RPC_LOCALSOCKET
CONFIG(Q_RPC_LOCALSOCKET){
QT += websockets
HEADERS += \
    $$PWD/private/p_qrpc_listen_local_socket.h

SOURCES += \
    $$PWD/private/p_qrpc_listen_local_socket.cpp
}

##declare to include CONFIG+=Q_RPC_TCP
CONFIG(Q_RPC_TCP){
QT += network
HEADERS += \
    $$PWD/private/p_qrpc_listen_tcp.h

SOURCES += \
    $$PWD/private/p_qrpc_listen_tcp.cpp
}

##declare to include CONFIG+=Q_RPC_UDP
CONFIG(Q_RPC_UDP){
QT += network
HEADERS += \
    $$PWD/private/p_qrpc_listen_udp.h

SOURCES += \
    $$PWD/private/p_qrpc_listen_udp.cpp
}


##obrigatory
CONFIG+=Q_RPC_HTTP
CONFIG(Q_RPC_HTTP){
QT += network
include($$PWD/3rdparty/qtwebapp/qtwebapp.pri)
HEADERS += \
    $$PWD/private/p_qrpc_listen_http.h \

SOURCES += \
    $$PWD/private/p_qrpc_listen_http.cpp \
}


HEADERS += \
    $$PWD/private/p_qrpc_listen_qrpc.h \
    $$PWD/private/p_qrpc_listen_qrpc_slot.h \
    $$PWD/qrpc_controller.h \
    $$PWD/qrpc_controller_router.h \
    $$PWD/qrpc_listen.h \
    $$PWD/qrpc_listen_request_parser.h \
    $$PWD/qrpc_listen_colletion.h \
    $$PWD/qrpc_listen_protocol.h \
    $$PWD/qrpc_listen_request_cache.h \
    $$PWD/qrpc_server.h \

SOURCES += \
    $$PWD/private/p_qrpc_listen_qrpc.cpp \
    $$PWD/private/p_qrpc_listen_qrpc_slot.cpp \
    $$PWD/qrpc_controller.cpp \
    $$PWD/qrpc_controller_router.cpp \
    $$PWD/qrpc_listen.cpp \
    $$PWD/qrpc_listen_request_parser.cpp \
    $$PWD/qrpc_listen_colletion.cpp \
    $$PWD/qrpc_listen_protocol.cpp \
    $$PWD/qrpc_listen_request_cache.cpp \
    $$PWD/qrpc_server.cpp \
