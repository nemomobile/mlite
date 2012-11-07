INCLUDEPATH += $${PWD}

HEADERS += \
    $${PWD}/qjson_p.h \
    $${PWD}/qjsondocument.h \
    $${PWD}/qjsonobject.h \
    $${PWD}/qjsonvalue.h \
    $${PWD}/qjsonarray.h \
    $${PWD}/qjsonwriter_p.h \
    $${PWD}/qjsonparser_p.h

SOURCES += \
    $${PWD}/qjson.cpp \
    $${PWD}/qjsondocument.cpp \
    $${PWD}/qjsonobject.cpp \
    $${PWD}/qjsonarray.cpp \
    $${PWD}/qjsonvalue.cpp \
    $${PWD}/qjsonwriter.cpp \
    $${PWD}/qjsonparser.cpp

INSTALL_HEADERS += \
    $${PWD}/qjsondocument.h \
    $${PWD}/qjsonobject.h \
    $${PWD}/qjsonvalue.h \
    $${PWD}/qjsonarray.h \
    $${PWD}/qjsonexport.h
