// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include <QByteArray>
#include "alkoserialreply.h"

AlkoSerialReply::AlkoSerialReply(QObject *parent) :
    SerialReply(parent)
{

}

void AlkoSerialReply::setData(const QByteArray &data)
{
    m_data = data;
    emit readyRead();
}
