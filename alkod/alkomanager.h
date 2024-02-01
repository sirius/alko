// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef ALKOMANAGER_H
#define ALKOMANAGER_H

#include <QFile>
#include "serialmanager.h"

class SerialReply;
class SerialRequest;
class HttpServer;
class CustomerHandler;
class QHostAddress;

class AlkoManager : public SerialManager
{
    Q_OBJECT
public:
    explicit AlkoManager(QObject *parent = 0);
    SerialReply * get(const SerialRequest & request);
    bool openMagtekDevice(const QString &magtekDevice);
    bool openSerialDevice(const QString &serialDevice);
    bool startHttpServer(const QHostAddress &address, quint16 port);
    void closeMagtekDevice();
    void closeSerialDevice();
    void stopHttpServer();
    void log(const QString &text, const QString &id = QString(), const QString &application = QString());
signals:
    
public slots:
    void slotReadyRead();
    void testWrite(const QByteArray &data);
    void autoStart();
private:
    uint8_t sizePending;
    uint16_t makeChecksum(uint16_t *buffer, uint32_t count);
    HttpServer *m_httpd;
    CustomerHandler *m_customerHandler;
    QFile logFile;
};

#endif // ALKOMANAGER_H
