// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef CUSTOMERHANDLER_H
#define CUSTOMERHANDLER_H

#include <QObject>

class AlkoManager;
class MagtekReader;
class QThread;

class CustomerHandler : public QObject
{
    Q_OBJECT
public:
    explicit CustomerHandler(AlkoManager *manager, QObject *parent = 0);
    ~CustomerHandler();
    bool open(const QString & magtekDevice);
    void close();
    bool isOpen() const { return m_isOpen; }
signals:
    
public slots:
    void newCustomer(const QString & data);
private:
    AlkoManager *m_manager;
    QThread *m_thread;
    MagtekReader *m_magtek;
    bool m_isOpen;
};

#endif // CUSTOMERHANDLER_H
