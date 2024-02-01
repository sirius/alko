// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef ALKO_STATES_H
#define ALKO_STATES_H

namespace Alko
{
    namespace State
    {
        const char SERVICE              = 0x01;
        const char PREPARE_READY        = 0x02;
        const char READY                = 0x03; 
        const char READY_WAIT           = 0x04;
        const char DEPLOY               = 0x05;
        const char DEPLOY_ERROR         = 0x06;
        const char PREPARE_AUTO_HOME    = 0x07;
        const char AUTO_HOME            = 0x08;
        const char INFO                 = 0x09;
    }
}

#endif // ALKO_STATES_H
