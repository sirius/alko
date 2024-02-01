// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef ALKOSERIALCOMMANDS_H
#define ALKOSERIALCOMMANDS_H

namespace Alko
{
    namespace Cmd
    {
        const char PING             = 0x00;
        const char PONG             = 0x01;
        const char BUSY             = 0x02;
        const char READY            = 0x03;
        const char SERVICE          = 0x04;
        const char GET_CONFIG       = 0x05;
        const char SET_CONFIG       = 0x06;
        const char GET_STATE        = 0x07;
        const char SET_STATE        = 0x08;
        const char HOMING           = 0x09;
        const char HOME_ABORT       = 0x0A;
        const char HOME_BEGIN       = 0x0B;
        const char CHECK_AVAIL      = 0x0C;
        const char DRINK_AVAIL      = 0x0D;
        const char EMPTY            = 0x0E;
        const char FETCH            = 0x0F;
    
        const char VND_COL_0        = 0x10;
        const char VND_COL_1        = 0x11;
        const char VND_COL_2        = 0x12;
        const char VND_COL_3        = 0x13;
        const char VND_COL_4        = 0x14;
        const char VND_COL_5        = 0x15;
        const char VND_COL_6        = 0x16;
        const char VND_COL_7        = 0x17;
        const char VND_COL_8        = 0x18;
        const char VND_COL_9        = 0x19;
    
        const char SET_TEXT_0       = 0x40;
        const char SET_TEXT_1       = 0x41;
        const char SET_TEXT_2       = 0x42;
        const char SET_TEXT_3       = 0x43;
        const char CLR_TEXT_0       = 0x44;
        const char CLR_TEXT_1       = 0x45;
        const char CLR_TEXT_2       = 0x46;
        const char CLR_TEXT_3       = 0x47;
        const char CLR_TEXT         = 0x48;
        const char SHOW_INFO        = 0x49;
    
        const char LCD_RGB          = 0x4C;
        const char LCD_RESET        = 0x4D;
        const char LCD_RED_CYCLE    = 0x4E;
        const char LCD_UPDATE       = 0x4F;
    
        const char UNKNOWN_CMD      = 0xFE;
        const char CHKSUM_INVALID   = 0xFF;
    }
}

#endif // ALKOSERIALCOMMANDS_H
