// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef ALKOSERIALREPLY_H
#define ALKOSERIALREPLY_H

#include "QObject"
#include "serialreply.h"

class AlkoSerialReply : public SerialReply
{
    Q_OBJECT
public:
    explicit AlkoSerialReply(QObject *parent = 0);
    uint8_t size() const { return m_data.size(); }
    const char * data() const { return m_data.constData(); }
    void setData(const QByteArray & data);
signals:
    
public slots:
    void emitReadyRead() { emit readyRead(); }
private:
    QByteArray m_data;
};

#endif // ALKOSERIALREPLY_H
