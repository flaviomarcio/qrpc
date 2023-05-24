QT += core
QT += network

##declare to include CONFIG+=Q_RPC_AMQP
CONFIG(Q_RPC_AMQP){
DEFINES+=Q_RPC_AMQP
HEADERS += \
    $$PWD/private/p_qrpc_request_job_amqp.h \

SOURCES += \
    $$PWD/private/p_qrpc_request_job_amqp.cpp \
}

##declare to include CONFIG+=Q_RPC_WEBSOCKET
CONFIG(Q_RPC_WEBSOCKET){
QT += websockets
DEFINES+=Q_RPC_WEBSOCKET
HEADERS += \
    $$PWD/private/p_qrpc_request_job_wss.h \

SOURCES += \
    $$PWD/private/p_qrpc_request_job_wss.cpp \
}

##declare to include CONFIG+=Q_RPC_DATABASE
CONFIG(Q_RPC_DATABASE){
QT += sql
DEFINES+=Q_RPC_DATABASE
HEADERS += \
    $$PWD/private/p_qrpc_request_job_database.h \

SOURCES+= \
    $$PWD/private/p_qrpc_request_job_database.cpp \
}

##declare to include CONFIG+=Q_RPC_TCP
CONFIG(Q_RPC_TCP){
DEFINES+=Q_RPC_TCP
HEADERS += \
    $$PWD/private/p_qrpc_request_job_tcp.h \

SOURCES += \
    $$PWD/private/p_qrpc_request_job_tcp.cpp \
}

##declare to include CONFIG+=Q_RPC_UDP
CONFIG(Q_RPC_UDP){
DEFINES+=Q_RPC_UDP
HEADERS += \
    $$PWD/private/p_qrpc_request_job_udp.h \

SOURCES += \
    $$PWD/private/p_qrpc_request_job_udp.cpp \
}

##declare to include CONFIG+=Q_RPC_LOCALSOCKET
CONFIG(Q_RPC_LOCALSOCKET){
DEFINES+=Q_RPC_LOCALSOCKET
HEADERS += \
    $$PWD/private/p_qrpc_request_job_localsocket.h \

SOURCES += \
    $$PWD/private/p_qrpc_request_job_localsocket.cpp \
}

##obrigatory
CONFIG+=Q_RPC_HTTP
CONFIG(Q_RPC_HTTP){
DEFINES+=Q_RPC_HTTP
HEADERS += \
    $$PWD/private/p_qrpc_request_job_http.h \

SOURCES += \
        $$PWD/private/p_qrpc_request_job_http.cpp \
}

HEADERS += \
    $$PWD/private/p_qrpc_http_headers.h \
    $$PWD/private/p_qrpc_http_response.h \
    $$PWD/private/p_qrpc_util.h \
    $$PWD/private/p_qrpc_request.h \
    $$PWD/private/p_qrpc_listen_request_code.h \
    $$PWD/private/p_qrpc_request_job.h \
    $$PWD/private/p_qrpc_request_job_protocol.h \
    $$PWD/private/p_qrpc_request_job_response.h \
    $$PWD/qrpc_listen_request.h \
    $$PWD/qrpc_request_exchange.h \
    $$PWD/qrpc_request_exchange_setting.h \
    $$PWD/qrpc_request.h


SOURCES += \
    $$PWD/private/p_qrpc_http_headers.cpp \
    $$PWD/private/p_qrpc_http_response.cpp \
    $$PWD/private/p_qrpc_listen_request_code.cpp \
    $$PWD/private/p_qrpc_request.cpp \
    $$PWD/private/p_qrpc_request_job.cpp \
    $$PWD/private/p_qrpc_request_job_protocol.cpp \
    $$PWD/private/p_qrpc_request_job_response.cpp \
    $$PWD/qrpc_listen_request.cpp \
    $$PWD/qrpc_request_exchange.cpp \
    $$PWD/qrpc_request_exchange_setting.cpp \
    $$PWD/qrpc_request.cpp \
    $$PWD/private/p_qrpc_util.cpp \
