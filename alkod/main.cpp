// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include <inttypes.h>
#include <QCoreApplication>
#include <QtNetwork/QHostAddress>
#include <QDebug>
#include "alkoservice.h"
#include "alkomanager.h"
#include "config.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    AlkoService service(argc, argv);

    if (argc != 3) {
        qDebug() << "Usage: " << argv[0] << " /dev/ttySerial /dev/input/magtek";
        return 0;
    }

    AlkoManager alko;

    if (!alko.openSerialDevice(argv[1])) {
        qDebug() << "Error: Could not open serial device at " << argv[1];
        return 1;
    }

    if (!alko.openMagtekDevice(argv[2])) {
        qDebug() << "Error: Could not open Magtek device at " << argv[2];
        //return 2;
        // Allow running without Magtek when testing
    }

    if (!alko.startHttpServer(QHostAddress::AnyIPv6, ALKO_CONFIGURATION__HTTP_PORT)) {
        qDebug() << "Error: Could not start HttpServer at " << QHostAddress(QHostAddress::AnyIPv6).toString() << ", port " << ALKO_CONFIGURATION__HTTP_PORT;
        return 3;
    }

    return a.exec();
}
