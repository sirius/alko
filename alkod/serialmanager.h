// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <inttypes.h>
#include <QObject>
#include <QHash>
#include <QtSerialPort/qserialport.h>

class SerialReply;
class SerialRequest;

class SerialManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialManager(QObject *parent = 0);
    void setPortName(const QString & name) { m_serialPort->setPortName(name); }
    bool open(QIODevice::OpenMode mode) { return m_serialPort->open(mode); }
    void close() { m_serialPort->close(); }
    virtual SerialReply * get(const SerialRequest & request) = 0;
    uint16_t getID();
signals:
    void readyRead();
public slots:
protected:
    QSerialPort *m_serialPort;
    QHash<quint16, SerialReply*> requests;
private:
    uint16_t m_id;

};

#endif // SERIALMANAGER_H
