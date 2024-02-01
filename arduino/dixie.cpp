// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include "dixie.h"
#include "datagram.h"
#include "../common/alkoserialcommands.h"
#include "config.h"

#define FIRST_DEPLOY_COLUMN 1
#define LAST_DEPLOY_COLUMN 9

Dixie::Dixie() :
    m_lcd(LCD_ADDRESS), m_gotPong(true), m_deployId(0), m_deployColumn(0),
    m_previousState(Alko::State::SERVICE), m_currentState(Alko::State::SERVICE), m_nextState(Alko::State::SERVICE)
{
    enterState = &Dixie::enterServiceState;
    updateState = &Dixie::updateServiceState;
    exitState = &Dixie::exitServiceState;

    m_pingTimer.setInterval(PING_TIMER);
    m_narcoTimer.setInterval(NARCO_TIMER);
    m_prepareReadyTimer.setInterval(PREPARE_READY_TIMER);
    m_motorStartTimer.setInterval(MOTOR_START_TIMER);
    m_motorRunTimer.setInterval(MOTOR_RUN_TIMER);
    m_deployErrorTimer.setInterval(DEPLOY_ERROR_TIMER);
    m_prepareAutoHomeTimer.setInterval(PREPARE_AUTO_HOME_TIMER);
    m_infoTimer.setInterval(INFO_TIMER);
}

void Dixie::begin()
{
    m_network.begin();
    m_narco.begin();
    m_lcd.reset();
    m_deployColumn = m_narco.getDeployColumn();
    (this->*enterState)();
}

// Main "event loop" / state machine
void Dixie::update()
{
    if (m_network.datagramIsPending()) {
        // This may change m_nextState
        datagramHandler(m_network.getPendingDatagram());
    }

    // Handle current state
    (this->*updateState)();

    if (m_currentState != m_nextState) {
        switchState();
    }
}

// --- SERVICE STATE ---

void Dixie::enterServiceState()
{
    m_narco.powerOff();

    m_lcd.reset();
    m_lcd.clear();
    m_lcd.setText(0, m_narco.getInfoText());
    m_lcd.setText(1, VERSION_STR);
    m_lcd.setText(2, SERVICE_TEXT_2_STR);
    m_lcd.setText(3, SERVICE_TEXT_3_STR);
    m_lcd.setColor(SERVICE_COLOR);
    m_lcd.update();

    m_narcoTimer.start(NARCO_TIMER);
}

void Dixie::updateServiceState()
{
    if (m_narcoTimer.timedOut()) {
        m_lcd.setText(0, m_narco.getInfoText());
        m_lcd.update();
        m_narcoTimer.start();
    }
}

void Dixie::exitServiceState()
{
    m_narcoTimer.stop();
}

// --- PREPARE_READY STATE ---

void Dixie::enterPrepareReadyState()
{
    m_narco.powerOff();

    m_prepareReadyTimer.start(PREPARE_READY_TIMER);

    m_lcd.clear();
    m_lcd.setText(2, PREPARE_READY_TEXT_2_STR);
    m_lcd.setText(3, PREPARE_READY_TEXT_3_STR);
    m_lcd.setColor(PREPARE_READY_COLOR);
    m_lcd.update();
}

void Dixie::updatePrepareReadyState()
{
    if (m_prepareReadyTimer.timedOut())
        setNextState(Alko::State::READY);
}
void Dixie::exitPrepareReadyState()
{
    m_prepareReadyTimer.stop();
}

// --- READY STATE ---

void Dixie::enterReadyState()
{
    m_pingTimer.start(PING_TIMER);

    m_lcd.clear();
    m_lcd.setText(0, m_narco.getInfoText());
    m_lcd.setText(1, VERSION_STR);
    m_lcd.setText(2, READY_TEXT_2_STR);
    m_lcd.setText(3, READY_TEXT_3_STR);
    m_lcd.setColor(READY_COLOR);
    m_lcd.update();

    m_narco.updateWithJam();
    m_narcoTimer.start();
}

void Dixie::updateReadyState()
{
    if (m_pingTimer.timedOut()) doPingPong();

    if (m_narcoTimer.timedOut()) {
        m_lcd.setText(0, m_narco.getInfoText());
        m_narcoTimer.start();
    }
}

void Dixie::exitReadyState()
{
    // NOP
}

// --- READY_WAIT STATE ---

void Dixie::enterReadyWaitState()
{
    m_lcd.clear();
    m_lcd.setText(0, m_narco.getInfoText());
    m_lcd.setText(1, VERSION_STR);
    m_lcd.setText(2, READY_WAIT_TEXT_2_STR);
    m_lcd.setText(3, READY_WAIT_TEXT_3_STR);
    m_lcd.setColor(READY_WAIT_COLOR);
    m_lcd.update();
}

void Dixie::updateReadyWaitState()
{
    if (m_narcoTimer.timedOut()) {
        m_lcd.setText(0, m_narco.getInfoText());
        m_narcoTimer.start();
    }
}

void Dixie::exitReadyWaitState()
{
    // NOP
}

// --- DEPLOY STATE ---

void Dixie::enterDeployState()
{
    m_lcd.setColor(DEPLOY_COLOR);
    m_lcd.update();
    m_deployColumn = m_narco.getDeployColumn();

    if (0 == m_deployColumn) {
        setNextState(Alko::State::PREPARE_READY);
        return;
    }

    char column = ((char) m_deployColumn);
    Datagram response(1);
    response.setId(m_deployId);
    response.setCommand(Alko::Cmd::FETCH);
    response.setData(&column, 1);
    m_network.sendDatagram(response);

    m_motorStartTimer.start();
    m_motorRunTimer.start();
    m_narco.powerOn(m_deployColumn);
}

void Dixie::updateDeployState()
{
    if (m_motorStartTimer.timedOut()) {
        if (m_narco.isHome(m_deployColumn))
            setNextState(Alko::State::PREPARE_READY);
        else if (m_motorRunTimer.timedOut())
            setNextState(Alko::State::DEPLOY_ERROR);
    }
}

void Dixie::exitDeployState()
{
    m_narco.powerOff();
    m_lcd.reset();
    m_deployId = 0;
}

// --- DEPLOY_ERROR STATE ---

void Dixie::enterDeployErrorState()
{
    m_narco.powerOff();

    m_narco.setJammed(m_deployColumn, true);

    m_lcd.clear();
    m_lcd.setText(0, DEPLOY_ERROR_TEXT_0_STR);
    m_lcd.setText(1, DEPLOY_ERROR_TEXT_1_STR);
    m_lcd.setText(2, DEPLOY_ERROR_TEXT_2_STR);
    m_lcd.setText(3, DEPLOY_ERROR_TEXT_3_STR);
    m_lcd.reset();
    m_lcd.setRedCycle();
    m_lcd.update();

    m_deployErrorTimer.start();
}

void Dixie::updateDeployErrorState()
{
    if (m_deployErrorTimer.timedOut()) setNextState(Alko::State::DEPLOY);
}

void Dixie::exitDeployErrorState()
{
    // NOP
}

// --- PREPARE_AUTO_HOME STATE ---

void Dixie::enterPrepareAutoHomeState()
{
    m_lcd.clear();
    m_lcd.setText(0, PREPARE_AUTO_HOME_TEXT_0_STR);
    m_lcd.setText(1, PREPARE_AUTO_HOME_TEXT_1_STR);
    m_lcd.setText(2, PREPARE_AUTO_HOME_TEXT_2_STR);
    m_lcd.setText(3, PREPARE_AUTO_HOME_TEXT_3_STR);
    m_lcd.reset();
    m_lcd.setRedCycle();
    m_lcd.update();

    m_prepareAutoHomeTimer.start();
}

void Dixie::updatePrepareAutoHomeState()
{
    if (m_prepareAutoHomeTimer.timedOut()) {
        Datagram response;
        response.setId(m_deployId);
        response.setCommand(Alko::Cmd::HOME_BEGIN);
        m_network.sendDatagram(response);
        m_deployColumn = FIRST_DEPLOY_COLUMN;
        setNextState(Alko::State::AUTO_HOME);
    }
}

void Dixie::exitPrepareAutoHomeState()
{
    // NOP
}

// --- AUTO_HOME STATE ---

void Dixie::enterAutoHomeState()
{
    // If previous state is not PREPARE_AUTO_HOME or AUTO_HOME, got to SERVICE state
    if ( (Alko::State::PREPARE_AUTO_HOME != m_previousState) && (Alko::State::AUTO_HOME != m_previousState) ) {
        setNextState(Alko::State::SERVICE);
        return;
    }

    char column = ((char) m_deployColumn);
    Datagram response(1);
    response.setId(m_deployId);
    response.setCommand(Alko::Cmd::HOMING);
    response.setData(&column, 1);
    m_network.sendDatagram(response);

    char _AUTO_HOME_TEXT_3_STR[21] = {'k','a','m','m','e','r',' ',(char)(m_deployColumn + '0'),'/','9','\0'};

    m_lcd.clear();
    m_lcd.setText(0, AUTO_HOME_TEXT_0_STR);
    m_lcd.setText(1, AUTO_HOME_TEXT_1_STR);
    m_lcd.setText(2, AUTO_HOME_TEXT_2_STR);
    m_lcd.setText(3, _AUTO_HOME_TEXT_3_STR);
    m_lcd.reset();
    m_lcd.setColor(AUTO_HOME_COLOR);
    m_lcd.update();

    m_motorRunTimer.start();
    m_narco.powerOn(m_deployColumn);
}

void Dixie::updateAutoHomeState()
{
    if (LAST_DEPLOY_COLUMN < m_deployColumn) {
        setNextState(Alko::State::SERVICE);
        return;
    }

    if (m_narco.isHome(m_deployColumn)) {
        m_narco.powerOff();
        m_deployColumn++;
        enterAutoHomeState();
    } else if (m_motorRunTimer.timedOut()) {
        m_narco.powerOff();
        m_narco.setJammed(m_deployColumn, true);
        m_deployColumn++;
        enterAutoHomeState();
    }
}

void Dixie::exitAutoHomeState()
{
    m_narco.powerOff();
}

// --- INFO STATE ---

void Dixie::enterInfoState()
{
    m_lcd.update();
    m_infoTimer.start();
}

void Dixie::updateInfoState()
{
    if (m_infoTimer.timedOut())
        setNextState(Alko::State::READY);
}

void Dixie::exitInfoState()
{
    // NOP
}

// Handle external requests
void Dixie::datagramHandler(Datagram dgr)
{
    if (Alko::State::READY_WAIT == m_currentState)
        setNextState(Alko::State::READY);

    switch (dgr.getCommand()) {
        case Alko::Cmd::PING:
        {
            Datagram response;
            response.setId(dgr.getId());
            if (Alko::State::READY == m_currentState)
                response.setCommand(Alko::Cmd::READY);
            else if (Alko::State::SERVICE == m_currentState)
                response.setCommand(Alko::Cmd::SERVICE);
            else
                response.setCommand(Alko::Cmd::BUSY);
            m_network.sendDatagram(response);
            break;
        }
        case Alko::Cmd::PONG:
            m_gotPong = true;
            break;
        case Alko::Cmd::GET_CONFIG:
        {
            Datagram response(4);
            response.setId(dgr.getId());
            response.setCommand(Alko::Cmd::GET_CONFIG);
            uint32_t config = m_narco.getRawData();
            char data[4];
            data[0] = ( (config >> 24) & 0xFF );
            data[1] = ( (config >> 16) & 0xFF );
            data[2] = ( (config >> 8) & 0xFF );
            data[3] = ( config & 0xFF );
            response.setData(data, 4);
            m_network.sendDatagram(response);
        }
            break;
        case Alko::Cmd::SET_CONFIG:
            break;
        case Alko::Cmd::GET_STATE:
            emitCurrentState(dgr.getId());
            break;
        case Alko::Cmd::SET_STATE:
        {
            Datagram response(1);
            if (1 != dgr.getDataSize()) { // Does not contain state data byte
                emitCurrentState(dgr.getId());
                return;
            }

            char new_state = *(dgr.getData());
            if (new_state == m_currentState) { // We will not re-enter current state
                emitCurrentState(dgr.getId());
                return;
            }

            switch (new_state) { // You may only request some states
                case Alko::State::READY: // Ok, but we'll go through PREPARE_READY
                    new_state = Alko::State::PREPARE_READY;
                    // Fallthrough
                case Alko::State::PREPARE_READY:
                case Alko::State::SERVICE:
                {
                    Datagram response(1);
                    response.setId(dgr.getId());
                    response.setCommand(Alko::Cmd::SET_STATE);
                    response.setData(dgr.getData(), 1);
                    m_network.sendDatagram(response);

                    setNextState(new_state);
                    break;
                }
                default:
                    emitCurrentState(dgr.getId());
                    break;
            }
            break;
        }
        case Alko::Cmd::HOME_ABORT:
        {
            Datagram response;
            response.setId(dgr.getId());
            if ( (Alko::State::PREPARE_AUTO_HOME == m_currentState) || (Alko::State::AUTO_HOME == m_currentState) ){
                response.setCommand(Alko::Cmd::HOME_ABORT);
                m_network.sendDatagram(response); // ACK abort
                setNextState(Alko::State::SERVICE);
                return;
            }
            // NAK abort, send current state
            emitCurrentState(dgr.getId());
            break;
        }
        case Alko::Cmd::HOME_BEGIN:
        {
            if (Alko::State::SERVICE != m_currentState) {
                emitCurrentState(dgr.getId()); // NAK home, we're not in service state
                return;
            } else {
                Datagram response;
                response.setId(dgr.getId());
                response.setCommand(Alko::Cmd::HOME_BEGIN);
                m_network.sendDatagram(response);
                setNextState(Alko::State::PREPARE_AUTO_HOME);
            }
            break;
        }
        case Alko::Cmd::CHECK_AVAIL:
        {
            Datagram response;
            response.setId(dgr.getId());
            if (m_narco.getDeployColumn())
                response.setCommand(Alko::Cmd::DRINK_AVAIL);
            else
                response.setCommand(Alko::Cmd::EMPTY);
            m_network.sendDatagram(response);
            break;
        }
        case Alko::Cmd::FETCH:
        {
            Datagram response;
            response.setId(dgr.getId());
            if (Alko::State::READY != m_currentState) {
                response.setCommand(Alko::Cmd::BUSY);
                m_network.sendDatagram(dgr);
                return;
            }

            if (m_narco.getDeployColumn()) {
                m_deployId = dgr.getId();
                setNextState(Alko::State::DEPLOY);
            } else {
                response.setCommand(Alko::Cmd::EMPTY);
                m_network.sendDatagram(response);
            }
            break;
        }
        case Alko::Cmd::SET_TEXT_0:
            m_lcd.setText(0, dgr.getData(), dgr.getDataSize());
            break;
        case Alko::Cmd::SET_TEXT_1:
            m_lcd.setText(1, dgr.getData(), dgr.getDataSize());
            break;
        case Alko::Cmd::SET_TEXT_2:
            m_lcd.setText(2, dgr.getData(), dgr.getDataSize());
            break;
        case Alko::Cmd::SET_TEXT_3:
            m_lcd.setText(3, dgr.getData(), dgr.getDataSize());
            break;
        case Alko::Cmd::CLR_TEXT:
            m_lcd.clear();
            break;
        case Alko::Cmd::SHOW_INFO:
            if (Alko::State::READY == m_currentState)
                setNextState(Alko::State::INFO);
            else
                dgr.setCommand(Alko::Cmd::BUSY);
            m_network.sendDatagram(dgr);
            break;
        case Alko::Cmd::LCD_RGB:
            if (3 == dgr.getDataSize()) {
                const char * colorByte = dgr.getData();
                char r = *colorByte++;
                char g = *colorByte++;
                char b = *colorByte;
                m_lcd.setColor(r,g,b);
            }
            break;
        case Alko::Cmd::LCD_UPDATE:
            m_lcd.update();
            break;
        case Alko::Cmd::LCD_RED_CYCLE:
            m_lcd.setRedCycle();
            break;
        default:
        {
            Datagram response;
            response.setId(dgr.getId());
            response.setCommand(Alko::Cmd::UNKNOWN_CMD);
            m_network.sendDatagram(response);
            break;
        }
    }
}

void Dixie::forceServiceState()
{
    if (Alko::State::SERVICE != m_currentState) {
        char data[2];
        data[0] = Alko::State::SERVICE;
        data[1] = 0xCE;

        Datagram response(2);
        response.setId(0);
        response.setCommand(Alko::Cmd::SET_STATE);
        response.setData(data, 2);
        m_network.sendDatagram(response);

        setNextState(Alko::State::SERVICE);
        switchState(); // Do it now, don't wait!
        m_lcd.setText(2, FORCE_SERVICE_TEXT_STR);
        m_lcd.update();
    }
}

void Dixie::doPingPong()
{
    if (!m_gotPong) {
        setNextState(Alko::State::READY_WAIT);
        return;
    }

    m_gotPong = false;
    m_network.sendCommand(Alko::Cmd::PING, 0);
    m_pingTimer.start();
}

void Dixie::emitCurrentState(uint16_t datagramId)
{
    char state = m_currentState;
    Datagram response(1);
    response.setCommand(Alko::Cmd::GET_STATE);
    response.setId(datagramId);
    response.setData(&state, 1);
    m_network.sendDatagram(response);
}

void Dixie::setNextState(const char state)
{
    m_nextState = state;
}

void Dixie::switchState()
{
    // Run cleanup for the state we are leaving
    (this->*exitState)();

    m_previousState = m_currentState;
    m_currentState = m_nextState;

    switch(m_nextState) {
    case Alko::State::SERVICE:
        enterState = &Dixie::enterServiceState;
        updateState = &Dixie::updateServiceState;
        exitState = &Dixie::exitServiceState;
        break;
    case Alko::State::PREPARE_READY:
        enterState = &Dixie::enterPrepareReadyState;
        updateState = &Dixie::updatePrepareReadyState;
        exitState = &Dixie::exitPrepareReadyState;
        break;
    case Alko::State::READY:
        enterState = &Dixie::enterReadyState;
        updateState = &Dixie::updateReadyState;
        exitState = &Dixie::exitReadyState;
        break;
    case Alko::State::READY_WAIT:
        enterState = &Dixie::enterReadyWaitState;
        updateState = &Dixie::updateReadyWaitState;
        exitState = &Dixie::exitReadyWaitState;
        break;
    case Alko::State::DEPLOY:
        enterState = &Dixie::enterDeployState;
        updateState = &Dixie::updateDeployState;
        exitState = &Dixie::exitDeployState;
        break;
    case Alko::State::DEPLOY_ERROR:
        enterState = &Dixie::enterDeployErrorState;
        updateState = &Dixie::updateDeployErrorState;
        exitState = &Dixie::exitDeployErrorState;
        break;
    case Alko::State::PREPARE_AUTO_HOME:
        enterState = &Dixie::enterPrepareAutoHomeState;
        updateState = &Dixie::updatePrepareAutoHomeState;
        exitState = &Dixie::exitPrepareAutoHomeState;
        break;
    case Alko::State::AUTO_HOME:
        enterState = &Dixie::enterAutoHomeState;
        updateState = &Dixie::updateAutoHomeState;
        exitState = &Dixie::exitAutoHomeState;
        break;
    case Alko::State::INFO:
        enterState = &Dixie::enterInfoState;
        updateState = &Dixie::updateInfoState;
        exitState = &Dixie::exitInfoState;
        break;
    default:
        break;
    }

    // Run initialization for the state we are entering
    (this->*enterState)();
}
