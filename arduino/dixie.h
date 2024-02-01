// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef DIXIE_H
#define DIXIE_H

#include "lcd.h"
#include "narco.h"
#include "network.h"
#include "datagram.h"
#include "timer.h"
#include "../common/alkostates.h"

class Dixie
{
public:
    Dixie();
    void begin();
    void update();
    void datagramHandler(Datagram dgr);
    void forceServiceState();
    void softwareReset() { asm volatile ("  jmp 0"); }

private:
    LCD m_lcd;
    Narco m_narco;
    Network m_network;

    Timer m_pingTimer;
    Timer m_narcoTimer;
    Timer m_prepareReadyTimer;
    Timer m_motorStartTimer;
    Timer m_motorRunTimer;
    Timer m_deployErrorTimer;
    Timer m_prepareAutoHomeTimer;
    Timer m_infoTimer;

    void doPingPong();
    bool m_gotPong;

    uint16_t m_deployId;
    uint8_t m_deployColumn;

    char m_previousState, m_currentState, m_nextState;
    void setNextState(const char state);
    void switchState();
    void emitCurrentState(uint16_t datagramId);

    void (Dixie::*enterState)();
    void (Dixie::*updateState)();
    void (Dixie::*exitState)();

    void nullFunction() {}

    void enterBootState();
    void updateBootState();
    void exitBootState();

    void enterServiceState();
    void updateServiceState();
    void exitServiceState();

    void enterPrepareReadyState();
    void updatePrepareReadyState();
    void exitPrepareReadyState();

    void enterReadyState();
    void updateReadyState();
    void exitReadyState();

    void enterReadyWaitState();
    void updateReadyWaitState();
    void exitReadyWaitState();

    void enterDeployState();
    void updateDeployState();
    void exitDeployState();

    void enterDeployErrorState();
    void updateDeployErrorState();
    void exitDeployErrorState();

    void enterPrepareAutoHomeState();
    void updatePrepareAutoHomeState();
    void exitPrepareAutoHomeState();

    void enterAutoHomeState();
    void updateAutoHomeState();
    void exitAutoHomeState();

    void enterInfoState();
    void updateInfoState();
    void exitInfoState();

};

#endif // DIXIE_H
