// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostAddress>

class AlkoManager;

class HttpServer : public QObject
{
    Q_OBJECT
public:
    explicit HttpServer(AlkoManager *manager, QObject *parent = 0);
    void setPort(quint16 port) { m_port = port; }
    void setListenAddress(QHostAddress address) { m_address = address; }

signals:
    
public slots:
    bool start();
    void stop();
    void pause();
    void resume();

private slots:
    void newConnection();

private:
    QTcpServer* m_tcpServer;
    quint16 m_port;
    QHostAddress m_address;
    bool m_disabled;
    AlkoManager *m_manager;
};

#endif // HTTPSERVER_H
