// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include "serialmanager.h"

SerialManager::SerialManager(QObject *parent) :
    QObject(parent), m_id(0)
{
    m_serialPort = new QSerialPort(this);
}

uint16_t SerialManager::getID()
{
    ++m_id;
    if (m_id < 2) //ID 0 and 1 are reserved
        m_id = 2;
    return m_id;
}
