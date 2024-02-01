// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef SERIALREQUEST_H
#define SERIALREQUEST_H

#include <QObject>
#include <inttypes.h>

class QByteArray;

class SerialRequest : public QObject
{
    Q_OBJECT
public:
    explicit SerialRequest(QObject *parent = 0);
    virtual uint8_t size() const = 0;
    virtual const char * data() const = 0;
signals:
    
public slots:
};

#endif // SERIALREQUEST_H
