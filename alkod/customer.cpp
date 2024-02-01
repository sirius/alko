// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include <QStringList>
#include <QByteArray>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QDebug>
#include <QtNetwork/QTcpSocket>
#include <QTimer>
#include <alkoserialreply.h>
#include <alkoserialrequest.h>
#include "customer.h"
#include "../common/alkoserialcommands.h"
#include "config.h"

Customer::Customer(const QString &customerID, AlkoManager *manager, QObject *parent) :
    QObject(parent), m_customerID(customerID), m_manager(manager)
{
    m_state = NewCustomer;
    connect(&m_timer, &QTimer::timeout, this, &Customer::timeout);
    connect(this, &Customer::done, this, &QObject::deleteLater);

    QTimer::singleShot(0, this, SLOT(handleCustomer()));
}

Customer::~Customer()
{
    if (m_socket)
        delete m_socket;
    if (m_reply)
        m_reply->deleteLater();
}

void Customer::handleCustomer()
{
    switch(m_state) {
    case NewCustomer: {
        //qDebug() << "State: NewCustomer";
        AlkoSerialRequest request;
        request.setData(QByteArray().append(Alko::Cmd::PING));
        m_reply = qobject_cast<AlkoSerialReply *>(m_manager->get(request));
        connect(m_reply, &AlkoSerialReply::readyRead, this, &Customer::handleCustomer);
        m_state = StateCheckDone;
        m_timer.start(250);
    }
    break;
    case StateCheckDone: {
        m_timer.stop();
        //qDebug() << "State: StateCheckDone";
        if (m_reply->data()[7] != Alko::Cmd::READY) { // Machine is busy
            if (m_reply->data()[7] == Alko::Cmd::SERVICE) {
                emit done();
                return;
            }
            m_reply->deleteLater();
            AlkoSerialRequest request;
            request.setData(QByteArray().append(Alko::Cmd::SET_TEXT_3).append("  Vent litt..."));
            m_manager->get(request);
            request.setData(QByteArray().append(Alko::Cmd::LCD_UPDATE));
            m_manager->get(request);
            m_state = AllDone;
            emit done();
            return;
        }

        m_reply->deleteLater();
        AlkoSerialRequest request;
        request.setData(QByteArray().append(Alko::Cmd::CHECK_AVAIL));
        m_reply = qobject_cast<AlkoSerialReply *>(m_manager->get(request));
        connect(m_reply, &AlkoSerialReply::readyRead, this, &Customer::handleCustomer);
        m_timer.start(250);
        m_state = SupplyCheckDone;
    }
    break;

    case SupplyCheckDone: {
        m_timer.stop();
        //qDebug() << "State: SupplyCheckDone";
        if (m_reply->data()[7] != Alko::Cmd::DRINK_AVAIL) { // Supplies are empty
            m_reply->deleteLater();
            m_state = AllDone;
            emit done();
            return;
        }

        m_reply->deleteLater();
        m_socket = new QTcpSocket;
        connect(m_socket, &QTcpSocket::readyRead, this, &Customer::handleCustomer);
        m_socket->connectToHost(QHostAddress(ALKO_CONFIGURATION__CUSTOMER_SERVER_ADDR), ALKO_CONFIGURATION__CUSTOMER_SERVER_PORT);
        QString checkonly = "no";
        if (m_customerID == ALKO_CONFIGURATION__CUSTOMER_DEBUG_ID) {//DEBUG
            checkonly = "yes";
        }
        m_socket->write(QString(ALKO_CONFIGURATION__CUSTOMER_SERVER_REQUEST)
                            .arg(m_customerID)
                            .arg(checkonly)
                            .toLocal8Bit());
        m_timer.start(500);
        m_state = PaymentCheckDone;
    }
    break;

    case PaymentCheckDone: {
        m_timer.stop();
        m_timer.start(250);
        //qDebug() << "State: PaymentCheckDone";
        QString reply = m_socket->readLine();
        m_manager->log(QString("From customer server: ").append(reply.trimmed()));
        bool ok = false;
        int result = reply.left(1).toInt(&ok);
        if (ok) {
            QStringList tokens = reply.trimmed().split(" ");
            switch(result) {
            case 1: { // OK, money paid
                AlkoSerialRequest request;
                request.setData(QByteArray().append(Alko::Cmd::CLR_TEXT));
                m_manager->get(request);
                request.setData(QByteArray().append(Alko::Cmd::SET_TEXT_3).append(tokens.last().rightJustified(19, ' ', true).toLatin1()));
                m_manager->get(request);
                request.setData(QByteArray().append(Alko::Cmd::SET_TEXT_2).append("Ny saldo: ").append(tokens[1].toLatin1()));
                m_manager->get(request);
                request.setData(QByteArray().append(Alko::Cmd::FETCH));
                m_manager->get(request);
            }
            break;
            case 2: { // Error, not enough money
                AlkoSerialRequest request;
                request.setData(QByteArray().append(Alko::Cmd::CLR_TEXT));
                m_manager->get(request);
                request.setData(QByteArray().append(Alko::Cmd::SET_TEXT_3).append(tokens.last().rightJustified(19, ' ', true).toLatin1()));
                m_manager->get(request);
                request.setData(QByteArray().append(Alko::Cmd::SET_TEXT_1).append("Saldo: ").append(tokens[1].toLatin1()));
                m_manager->get(request);
                request.setData(QByteArray().append(Alko::Cmd::SET_TEXT_2).append("For lav saldo."));
                m_manager->get(request);
                request.setData(QByteArray().append(Alko::Cmd::SHOW_INFO));
                m_manager->get(request);
                request.setData(QByteArray().append(Alko::Cmd::LCD_RED_CYCLE));
                m_manager->get(request);
            }
            break;
            case 3: { // Error, unknown card
                AlkoSerialRequest request;
                request.setData(QByteArray().append(Alko::Cmd::CLR_TEXT));
                m_manager->get(request);
                request.setData(QByteArray().append(Alko::Cmd::SET_TEXT_2).append("Ukjent kort!"));
                m_manager->get(request);
                request.setData(QByteArray().append(Alko::Cmd::SHOW_INFO));
                m_manager->get(request);
                request.setData(QByteArray().append(Alko::Cmd::LCD_RED_CYCLE));
                m_manager->get(request);
            }
            break;
            case 6: {
                AlkoSerialRequest request;
                request.setData(QByteArray().append(Alko::Cmd::CLR_TEXT));
                m_manager->get(request);
                request.setData(QByteArray().append(Alko::Cmd::SET_TEXT_3).append(tokens.last().rightJustified(19, ' ', true).toLatin1()));
                m_manager->get(request);
                request.setData(QByteArray().append(Alko::Cmd::SET_TEXT_2).append("Ny saldo: ").append(tokens[1].toLatin1()));
                m_manager->get(request);
                request.setData(QByteArray().append(Alko::Cmd::SET_TEXT_1).append(QString("DEBUG - TESTKJØP").toLatin1()));
                m_manager->get(request);
                request.setData(QByteArray().append(Alko::Cmd::FETCH));
                m_manager->get(request);
            }
            break;
                // TODO: Add error handling of 4, 5, 6
            default:
                break;
            }

            m_timer.stop();
            m_state = AllDone;
            emit done();
            return;
        }
    }
    break;

    default:
        break;
    }
}

void Customer::timeout()
{
    qDebug() << "Timed out from state " << m_state;
    m_state = Expired;
    m_timer.stop();
    emit done();
}
