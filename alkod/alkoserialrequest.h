// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef ALKOSERIALREQUEST_H
#define ALKOSERIALREQUEST_H

#include <QObject>
#include "serialrequest.h"

class QByteArray;

class AlkoSerialRequest : public SerialRequest
{
    Q_OBJECT
public:
    explicit AlkoSerialRequest(QObject *parent = 0);
    AlkoSerialRequest & operator=(const AlkoSerialRequest &other);
    uint8_t size() const { return m_data.size(); }
    const char * data() const { return m_data.constData(); }
    void setData(const QByteArray & data) { m_data = data; }
signals:
    
public slots:
private:
    QByteArray m_data;
};

#endif // ALKOSERIALREQUEST_H
