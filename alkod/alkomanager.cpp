// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include <arpa/inet.h>
#include <QDataStream>
#include <QTimer>
#include <QDateTime>
#include <QTextStream>
#include <QtSerialPort>
#include <QDebug>
#include "config.h"
#include "alkomanager.h"
#include "alkoserialreply.h"
#include "alkoserialrequest.h"
#include "customerhandler.h"
#include "httpserver.h"
#include "../common/alkoserialcommands.h"

AlkoManager::AlkoManager(QObject *parent):
    SerialManager(parent)
{
    sizePending = 0; 
    m_customerHandler = new CustomerHandler(this, this);
    m_httpd = new HttpServer(this, this);
    connect(m_serialPort, &QSerialPort::readyRead, this, &AlkoManager::slotReadyRead);

    logFile.setFileName(ALKO_CONFIGURATION__LOG_FILE);
    if (!logFile.open(QIODevice::Append)) {
        qDebug() << "*** Error: could not open log file:" << ALKO_CONFIGURATION__LOG_FILE;
    }

    QTimer::singleShot(3000, this, SLOT(autoStart()));
}

void AlkoManager::autoStart()
{
    AlkoSerialRequest request;
    request.setData(QByteArray().append(Alko::Cmd::PING));
    this->get(request); 
}

bool AlkoManager::openMagtekDevice(const QString &magtekDevice)
{
    return m_customerHandler->open(magtekDevice);
}

bool AlkoManager::openSerialDevice(const QString &serialDevice)
{
    setPortName(serialDevice);
    return open(QIODevice::ReadWrite);
}

bool AlkoManager::startHttpServer(const QHostAddress &address, quint16 port)
{
    m_httpd->setListenAddress(address);
    m_httpd->setPort(port);
    return m_httpd->start();
}

void AlkoManager::closeMagtekDevice()
{
    m_customerHandler->close();
}

void AlkoManager::closeSerialDevice()
{
    m_serialPort->close();
}

void AlkoManager::stopHttpServer()
{
    m_httpd->stop();
}

void AlkoManager::log(const QString &text, const QString &id, const QString &application)
{
    QTextStream logstr(&logFile);
    logstr << QDateTime::currentDateTime().toString("yyyy-MM-dd hh.mm.ss,zzz") 
           << QString(" *** [%1] [%2] ").arg(id).arg(application)
           << text << "\n";
}

SerialReply *AlkoManager::get(const SerialRequest &request)
{
    uint16_t id = getID();
    //qDebug() << "ID is " << id;
    uint16_t checksum = 0;

    //Add serialReply with ID to list
    AlkoSerialReply * reply = new AlkoSerialReply();


    QByteArray data;
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream.setVersion(QDataStream::Qt_5_0);
    stream.setByteOrder(QDataStream::BigEndian);

    //Signature bytes, size, ID, checksum and data
    stream << (uint8_t) 0x42
           << (uint8_t) 0x53
           << (uint8_t) 0x08
           << (uint16_t) id
           << (uint16_t) checksum;

    QByteArray requestData(request.data(), request.size());

    for (int i = 0; i < request.size(); ++i)
        stream << (uint8_t) requestData.at(i);

    //Set size
    stream.device()->seek(2);
    stream << (uint8_t) data.size();

    //Calculate checksum
    checksum = makeChecksum((uint16_t *) data.constData(), data.size());
    stream.device()->seek(5);
    stream << (uint16_t) checksum;

    if (makeChecksum((uint16_t *) data.constData(), data.size()) != 0)
        qDebug() << "ERROR: Checksum is NOT OK!";

    //Send data
    m_serialPort->write(data.data(), data.size());

    log(QString("SEND [%1]").arg(QString(data.toHex())));

    // Add to request list
    requests.insert(id, reply);

    return reply;
}

void AlkoManager::slotReadyRead()
{
    if (sizePending == 0) {
        if (m_serialPort->bytesAvailable() < 3) //We need signature and length
            return;

        uint8_t sig;
        sig = m_serialPort->read(1).at(0);
        if (sig != 0x42) //First signature byte
            return;

        sig = m_serialPort->read(1).at(0);
        if (sig != 0x53) //Second signature byte
            return;

        sizePending = (uint8_t) m_serialPort->read(1).at(0);
        if (sizePending > 64 || sizePending < 8) {
            sizePending = 0; //Too large or small datagram
            return;
        }

        sizePending -= 3; //We have already read three bytes
    }

    if (m_serialPort->bytesAvailable() < sizePending) //Wait for all data
        return;

    QByteArray datagram;
    datagram.append(0x42).append(0x53).append(sizePending + 3) //We have already read three bytes
            .append(m_serialPort->read(sizePending));

    log(QString("RECV [%1]").arg(QString(datagram.toHex())));

    if (makeChecksum((uint16_t *) datagram.constData(), datagram.size()) != 0)
        qDebug() << "ERROR: Checksum is NOT OK!";

    sizePending = 0; //Reset

	// Reply to ping
    if (Alko::Cmd::PING == datagram[7]) {
		AlkoSerialRequest request;
        request.setData(QByteArray().append(Alko::Cmd::PONG));
    	this->get(request);
	}

    uint16_t id;
    id = (datagram[3] << 8) + datagram[4];

    if (requests.contains(id)) {
        AlkoSerialReply * reply = qobject_cast<AlkoSerialReply *>(requests.value(id));
	reply->setData(datagram);
    }
}

void AlkoManager::testWrite(const QByteArray &data)
{
    AlkoSerialRequest request;
    request.setData(data);
    this->get(request);
}

uint16_t AlkoManager::makeChecksum(uint16_t *buffer, uint32_t count)
{
    uint32_t sum = 0;

    while (count > 1) {
        sum += *buffer++;
        count -= 2;
    }

    if (count > 0)
        sum += *(uint8_t *)buffer;

    while (sum>>16)
        sum = (sum & 0xffff) + (sum >> 16);

    return (uint16_t)(~sum);
}
