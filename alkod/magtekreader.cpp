// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include "magtekreader.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>
#include <QDebug>

MagtekReader::MagtekReader(QObject *parent)
    : QObject(parent),
      fevdev(-1),
      size(0),
      value(0)
{
    size = sizeof(struct input_event);
}

bool MagtekReader::openDevice(QString device)
{
    int errorcode = 0;
    // Close previously used device, if any
    closeDevice();

    //Get file handler
    fevdev = open(device.toStdString().data(), O_RDONLY);
    if (fevdev == -1) {
        qDebug() << "Error getting fevdev";
        return false;
    }

    //Getting name of device
    char name[256] = "Unknown";
    if (ioctl(fevdev, EVIOCGNAME(sizeof(name)), name))
        device_name = name;

    //Getting exclusive access
    if ((errorcode = ioctl(fevdev, EVIOCGRAB, 1)))
    {
        qDebug() << "Error getting exclusive access. Error code: " << errorcode;
        return false;
    }

    return true;
}

void MagtekReader::closeDevice()
{
    if (fevdev != -1) {
        ioctl(fevdev, EVIOCGRAB, 1);
        fevdev = -1;
    }
}



const QString MagtekReader::readFromDevice()
{
    QString data;
    int c;
    while (1)
    {
        if (read(fevdev, ev, size * 64) < size) {
            break;
        }

        value = ev[0].value;

        if (value != ' ' && ev[1].value == 1 && ev[1].type == 1) {
            c = ev[1].code + 47;
            if (c == 75) {
                emit gotDataFromDevice(data);
                return data;
                //continue;
                }
            if (c == 86 || c == 89)
                continue;
            if (c == 58)
                c -= 10;
            data.append(c);
        }
    }
    emit gotDataFromDevice(data);
    return data;
}

void MagtekReader::readFromDeviceSignalOnly()
{
    QString data;
    int c;
    while (1)
    {
        if (read(fevdev, ev, size * 64) < size) {
            break;
        }

        value = ev[0].value;

        if (value != ' ' && ev[1].value == 1 && ev[1].type == 1) {
            c = ev[1].code + 47;
            if (c == 75) {
                emit gotDataFromDevice(data);
                data.clear();
                continue;
                }
            if (c == 86 || c == 89)
                continue;
            if (c == 58)
                c -= 10;
            data.append(c);
        }
    }
    emit gotDataFromDevice(data);
}



