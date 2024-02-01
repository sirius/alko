// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <QTimer>
#include "alkomanager.h"

class QTcpSocket;
class AlkoSerialReply;

class Customer : public QObject
{
    Q_OBJECT
public:
    enum State {
        NewCustomer,
        StateCheckDone,
        SupplyCheckDone,
        PaymentCheckDone,
        AllDone,
        Expired
    };

    explicit Customer(const QString &customerID, AlkoManager *manager, QObject *parent = 0);
    ~Customer();
signals:
    void done();
private:
    QString m_customerID;
    State m_state;
    QTcpSocket *m_socket;
    AlkoSerialReply *m_reply;
    AlkoManager *m_manager;
    QTimer m_timer;
private slots:
    void handleCustomer();
    void timeout();
};

#endif // CUSTOMER_H
