// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include "alkoservice.h"
#include "config.h"

AlkoService::AlkoService(int argc, char **argv) :
    QtService<QCoreApplication>(argc, argv, QString(ALKO_CONFIGURATION__SERVICE_NAME))
{
    setServiceDescription(ALKO_CONFIGURATION__SERVICE_DESCRIPTION);
    setServiceFlags(QtServiceBase::Default);
}

void AlkoService::start()
{
}

void AlkoService::pause()
{
}

void AlkoService::resume()
{
}
