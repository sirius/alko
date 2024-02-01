// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include <QByteArray>
#include "alkoserialrequest.h"

AlkoSerialRequest::AlkoSerialRequest(QObject *parent) :
    SerialRequest(parent)
{
}

AlkoSerialRequest & AlkoSerialRequest::operator=(const AlkoSerialRequest &other)
{
    m_data = other.m_data;
    return *this;
}
