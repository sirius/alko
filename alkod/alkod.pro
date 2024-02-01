# SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
# SPDX-License-Identifier: Apache-2.0

CONFIG	+= console
QT	-= gui
QT	+= serialport network
TEMPLATE = app
TARGET = alkod
INCLUDEPATH += . ../qtservice/src
LIBS += -L$$PWD/../qtservice/lib/ -lqtservice

target.path = $$PREFIX/bin
INSTALLS += target

# Input
HEADERS += \
    config.h \
    alkoserialreply.h \
    alkoserialrequest.h \
    magtekreader.h \
    serialmanager.h \
    serialreply.h \
    serialrequest.h \
    ../common/alkoserialcommands.h \
    alkomanager.h \
    httpserver.h \
    httphandler.h \
    customerhandler.h \
    customer.h \
    alkoservice.h

SOURCES += \
    alkoserialreply.cpp \
    alkoserialrequest.cpp \
    magtekreader.cpp \
    main.cpp \
    serialmanager.cpp \
    serialreply.cpp \
    serialrequest.cpp \
    alkomanager.cpp \
    httpserver.cpp \
    httphandler.cpp \
    customerhandler.cpp \
    customer.cpp \
    alkoservice.cpp

OTHER_FILES += \
    html/index.html \
    html/process.html \
    html/log.html \
    html/admin.html \
    html/rawlog.html \
    html/logout.html \
    html/footer.html.inc \
    html/nav.html.inc \
    html/header1.html.inc \
    html/header2.html.inc \
    html/status.html \
    html/check_status.html

RESOURCES += \
    resources.qrc
