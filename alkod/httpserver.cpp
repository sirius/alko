// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include "httpserver.h"
#include "httphandler.h"

HttpServer::HttpServer(AlkoManager *manager, QObject *parent) :
    QObject(parent), m_port(0), m_address(QHostAddress::Any), m_disabled(false), m_manager(manager)
{
    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer, &QTcpServer::newConnection, this, &HttpServer::newConnection);
}

bool HttpServer::start()
{
    if (m_tcpServer->isListening())
        return true;

    return m_tcpServer->listen(m_address, m_port);
}

void HttpServer::stop()
{
    m_tcpServer->close();
}

void HttpServer::pause()
{
    m_disabled = true;
}

void HttpServer::resume()
{
    m_disabled = false;
}

void HttpServer::newConnection()
{
    if (m_disabled)
        return;
    new HttpHandler(m_tcpServer->nextPendingConnection(), m_manager, this);
}
