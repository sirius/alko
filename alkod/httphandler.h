// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QTextStream>
class AlkoManager;

class HttpHandler : public QObject
{
    Q_OBJECT
public:
    explicit HttpHandler(QTcpSocket *socket, AlkoManager *manager, QObject *parent = 0);
    
signals:
    
public slots:

private slots:
    void readyRead();
    void error_400();
    void error_404();

private:
    QTcpSocket *m_socket;
    QTextStream m_socketOutputStream;
    AlkoManager *m_manager;
};

#endif // HTTPHANDLER_H
