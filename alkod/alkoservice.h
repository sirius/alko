// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef ALKOSERVICE_H
#define ALKOSERVICE_H

#include <QCoreApplication>
#include "../qtservice/src/qtservice.h"

class AlkoService : public QtService<QCoreApplication>
{

public:
    AlkoService(int argc, char **argv);

protected:
    void start();
    void pause();
    void resume();

};

#endif // ALKOSERVICE_H
