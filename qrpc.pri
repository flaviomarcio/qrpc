QT += core
QT += network
QT += websockets

QTREFORCE_QRPC=true
DEFINES+=QTREFORCE_QRPC

INCLUDEPATH += $$PWD/includes

HEADERS += \
    $$PWD/includes/QRpc/Client \
    $$PWD/includes/QRpc/Server

#source modules
QRPC_BASE_SOURCE=$$PWD/src/qrpc-base.pri
QRPC_TOKEN_SOURCE=$$PWD/src/qrpc-token.pri
QRPC_CLIENT_SOURCE=$$PWD/src/qrpc-client.pri
QRPC_SERVER_SOURCE=$$PWD/src/qrpc-server.pri

#check source exists
exists($$QRPC_BASE_SOURCE){QRPC_BASE=true} else {QRPC_BASE=false}
exists($$QRPC_TOKEN_SOURCE){QRPC_TOKEN=true} else {QRPC_TOKEN=false}
exists($$QRPC_CLIENT_SOURCE){QRPC_CLIENT=true} else {QRPC_CLIENT=false}
exists($$QRPC_SERVER_SOURCE){QRPC_SERVER=true} else {QRPC_SERVER=false}

#exclued modules apply

CONFIG(QTREFORCE_QRPC_NO_SERVER) {QRPC_SERVER=false} else {QRPC_SERVER=true}
CONFIG(QTREFORCE_QRPC_NO_CLIENT) {QRPC_CLIENT=false} else {QRPC_CLIENT=true}

#avaliable common modules
QRPC_CLISRV=false
equals(QRPC_CLIENT,true):QRPC_CLISRV=true
equals(QRPC_SERVER,true):QRPC_CLISRV=true

#client/server modules apply
equals(QRPC_CLISRV,true){QRPC_BASE=true} else {QRPC_BASE=false}
equals(QRPC_CLISRV,true){QRPC_TOKEN=true} else {QRPC_TOKEN=false}
equals(QRPC_CLIENT,true){QRPC_CLIENT=true} else {QRPC_CLIENT=false}
equals(QRPC_SERVER,true){QRPC_SERVER=true} else {QRPC_SERVER=false}

#include sources
equals(QRPC_BASE,true): include($$QRPC_BASE_SOURCE)
equals(QRPC_TOKEN,true): include($$QRPC_TOKEN_SOURCE)
equals(QRPC_CLIENT,true): include($$QRPC_CLIENT_SOURCE)
equals(QRPC_SERVER,true): include($$QRPC_SERVER_SOURCE)

#directives declare
equals(QRPC_CLISRV,true): DEFINES+=QRPC_CLISRV

equals(QRPC_BASE,true): DEFINES+=QRPC_BASE
equals(QRPC_TOKEN,true): DEFINES+=QRPC_TOKEN
equals(QRPC_CLIENT,true): DEFINES+=QRPC_CLIENT
equals(QRPC_SERVER,true): DEFINES+=QRPC_SERVER


message(" ")
message(-QTREFORCE QRCP-MODULES-DEFINITION-ACCEPT)
message("   -MODULES")
equals(QRPC_BASE,true)      { message("      +QRPC_BASE............... accepted") } else { message("      +QRPC_BASE................. ignored") }
equals(QRPC_TOKEN,true)     { message("      +QRPC_TOKEN.............. accepted") } else { message("      +QRPC_TOKEN................ ignored") }
equals(QRPC_CLIENT,true)    { message("      +QRPC_CLIENT............. accepted") } else { message("      +QRPC_CLIENT............... ignored") }
equals(QRPC_SERVER,true)    { message("      +QRPC_SERVER............. accepted") } else { message("      +QRPC_SERVER............... ignored") }
message("   -SOURCES")
equals(QRPC_BASE,true)      { message("      +QRPC_BASE............... "$$QRPC_BASE_SOURCE) }
equals(QRPC_TOKEN,true)     { message("      +QRPC_TOKEN.............. "$$QRPC_TOKEN_SOURCE) }
equals(QRPC_CLIENT,true)    { message("      +QRPC_CLIENT............. "$$QRPC_CLIENT_SOURCE) }
equals(QRPC_SERVER,true)    { message("      +QRPC_SERVER............. "$$QRPC_SERVER_SOURCE) }


INCLUDEPATH+=$$PWD/includes

HEADERS += \
    $$PWD/includes/QRpc/Client \
    $$PWD/includes/QRpc/Server

RESOURCES += \
    $$PWD/src/qrpc.qrc
