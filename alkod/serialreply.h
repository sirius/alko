// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef SERIALREPLY_H
#define SERIALREPLY_H

#include <inttypes.h>
#include <QObject>

class QByteArray;

class SerialReply : public QObject
{
    Q_OBJECT
public:
    explicit SerialReply(QObject *parent = 0);
    virtual uint8_t size() const = 0;
    virtual const char * data() const = 0;
signals:
    void readyRead();
public slots:
};

#endif // SERIALREPLY_H
