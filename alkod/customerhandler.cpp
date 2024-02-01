// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include <QThread>
#include <QStringList>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include "customerhandler.h"
#include "customer.h"
#include "magtekreader.h"
#include "alkomanager.h"

CustomerHandler::CustomerHandler(AlkoManager *manager, QObject *parent) :
    QObject(parent), m_manager(manager), m_isOpen(false)
{
}

CustomerHandler::~CustomerHandler()
{
    if (m_thread)
        m_thread->terminate();
}

bool CustomerHandler::open(const QString &magtekDevice)
{
    m_magtek = new MagtekReader();
    if (!m_magtek->openDevice(magtekDevice)) {
        delete m_magtek;
        return false;
    }

    m_thread = new QThread(this);

    connect(m_magtek, &MagtekReader::gotDataFromDevice, this, &CustomerHandler::newCustomer, Qt::QueuedConnection);
    connect(m_thread, &QThread::started, m_magtek, &MagtekReader::readFromDeviceSignalOnly);
    connect(m_thread, &QThread::finished, m_magtek, &MagtekReader::deleteLater);

    m_magtek->moveToThread(m_thread);
    m_thread->start();

    return m_isOpen = true;
}

void CustomerHandler::close()
{
    if (m_thread) {
        m_thread->terminate();
        delete m_thread;
    }

    if (m_magtek) {
        m_magtek->closeDevice();
        delete m_magtek;
    }

    m_isOpen = false;
}

void CustomerHandler::newCustomer(const QString &data)
{
    QString d = data;
    if (data.length() == 16) //RFID-card
        d = data.right(10);
    if (data.length() == 12)
        d = data.right(6);
    m_manager->log(QString("New customer: %1").arg(d));

    new Customer(d, m_manager);
}
