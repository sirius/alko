// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef MAGTEKREADER_H
#define MAGTEKREADER_H

#include <linux/input.h>
#include <QObject>
#include <QString>

class MagtekReader : public QObject
{
    Q_OBJECT
public:
    explicit MagtekReader(QObject *parent = 0);
    bool openDevice(QString device);
    void closeDevice();
public slots:
    const QString readFromDevice();
    void readFromDeviceSignalOnly();
    const QString deviceName() { return device_name; }
private:
    QString input_device;
    QString device_name;
    struct input_event ev[64];
    int fevdev;
    int size;
    int value;
signals:
    void gotDataFromDevice(const QString & data);
};

#endif // MAGTEKREADER_H
