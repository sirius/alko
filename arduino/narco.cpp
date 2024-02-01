// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include <Arduino.h>
#include <Wire.h>
#include <string.h>
#include "narco.h"

/*
 * MCP23017 connection table
 * Using two 16 bit i/o expanders, configured on 0x20 and 0x21
 * BTN = push buttons
 * MTR = column vender motors power (via relay)
 * MTC = column vender rotation switch - signals a half or a full rotation is complete
 * SOS = column sold out switch
 * +---+----------+----------+----------+----------+
 * |   | 0x20 GPA | 0x20 GPB | 0x21 GPA | 0x21 GPB |
 * +---+----------+----------+----------+----------+
 * | 7 | BTN-5    | MTR-8    | MTC-1    | MTC-9    |
 * | 6 | BTN-4    | MTR-7    | SOS-9    | MTC-8    |
 * | 5 | BTN-3    | MTR-6    | SOS-8    | MTC-7    |
 * | 4 | BTN-2    | MTR-5    | SOS-7    | MTC-6    |
 * | 3 | BTN-1    | MTR-4    | SOS-6    | MTC-5    |
 * | 2 | MTR-9    | MTR-3    | SOS-5    | MTC-4    |
 * | 1 | SOS-2    | MTR-2    | SOS-4    | MTC-3    |
 * | 0 | SOS-1    | MTR-1    | SOS-3    | MTC-2    |
 * +---+----------+----------+----------+----------+
 *
 */

Narco::Narco() :
	m_narco(0)
{
	strncpy(m_infoText, "    [_________]", sizeof(m_infoText) / sizeof(m_infoText[0]));
}

void Narco::begin()
{
	m_mcp0.begin(0);
	m_mcp1.begin(1);
	// Set pull-ups on all inputs
	for (int i = 0; i < 16; ++i) {
		m_mcp0.pinMode(i, INPUT);
		m_mcp0.pullUp(i, HIGH);
		m_mcp1.pinMode(i, INPUT);
		m_mcp1.pullUp(i, HIGH);
	}

	// MTR-1 to MTR-8
	for (int i = 0; i < 8; ++i) {
		m_mcp0.pinMode(i + 8, OUTPUT);
		m_mcp0.pullUp(i + 8, LOW);
	}

	// MTR-9
	m_mcp0.pinMode(2, OUTPUT);
	m_mcp0.pullUp(2, LOW);

	// Setup interrupt
	Wire.beginTransmission(0x21);
	Wire.write(MCP23017_GPINTENA);
	Wire.write(0x1 << 7);
	Wire.endTransmission();

	Wire.beginTransmission(0x21);
    Wire.write(MCP23017_GPINTENB);
    Wire.write(0xFF);
    Wire.endTransmission();

    Wire.beginTransmission(0x21);
    Wire.write(MCP23017_IOCONA);
    Wire.write( (0x1 << 6));
    Wire.endTransmission();

	powerOff();
}

void Narco::update()
{
    // Update SOS for all columns
	if (!m_mcp0.digitalRead(0)) // Column 1
		m_narco |= 1;
	else
		m_narco &= ~(1);

	if (!m_mcp0.digitalRead(1)) // Column 2
		m_narco |= (1 << 1);
	else
		m_narco &= ~(1 << 1);

	for (int i = 2; i < 9; ++i)
		if (!m_mcp1.digitalRead(i - 2)) // Column 3 - 9
			m_narco |= (1 << i);
		else
			m_narco &= ~(1 << i);
}

void Narco::updateWithJam()
{
	update();

	// Check motor state
	for (int column = NARCO_FIRST_COLUMN; column <= NARCO_LAST_COLUMN; ++column)
		if (isHome(column)) // Motor 1 - 9
			setJammed(column, false);
		else
			setJammed(column, true);

}

bool Narco::isFull(int column)
{
	if (column < NARCO_FIRST_COLUMN || column > NARCO_LAST_COLUMN) return false;
	return ((m_narco >> (column - 1)) & 0x01);
}

void Narco::setFull(int column, bool b)
{
	if (column < NARCO_FIRST_COLUMN || column > NARCO_LAST_COLUMN) return;
	if (b)
		m_narco |= (1 << (column - 1));
	else
		m_narco &= ~(1 << (column - 1));
}

bool Narco::isJammed(int column)
{
    if (column < NARCO_FIRST_COLUMN || column > NARCO_LAST_COLUMN) return false;
	return ((m_narco >> (column + 8)) & 0x01);
}

void Narco::setJammed(int column, bool b)
{
    if (column < NARCO_FIRST_COLUMN || column > NARCO_LAST_COLUMN) return;
	if (b)
		m_narco |= (1L << (column + 8));
	else
		m_narco &= ~(1L << (column + 8));
}

bool Narco::isHome(int column)
{
    if (column < NARCO_FIRST_COLUMN || column > NARCO_LAST_COLUMN) return false;
	return !m_mcp1.digitalRead(column + 6); // Motor 1 - 9
}

void Narco::powerOn(int column)
{
	powerOff();
    if (column < NARCO_FIRST_COLUMN || column > NARCO_LAST_COLUMN) return;
	if (NARCO_LAST_COLUMN == column)
		m_mcp0.digitalWrite(2, HIGH);
	else
		m_mcp0.digitalWrite(column + 7, HIGH);
}

void Narco::powerOff()
{
	for (int i = 8; i < 16; ++i)
		m_mcp0.digitalWrite(i, LOW);
	m_mcp0.digitalWrite(2, LOW);
}

void Narco::homeAllMotors()
{
	for (int column = NARCO_FIRST_COLUMN; column <= NARCO_LAST_COLUMN; ++column) {
		if (isHome(column))
			continue;
		powerOn(column);
		delay(2000);
		while (!isHome(column));
		powerOff();
	}
}

uint8_t Narco::getDeployColumn()
{
	updateWithJam();
	uint8_t currentColumn = (uint8_t)((m_narco >> 18) & 0x0F);

	if (0 == currentColumn) currentColumn = NARCO_FIRST_COLUMN;

	for (int column = currentColumn; column <= NARCO_LAST_COLUMN; ++column) {
		if (isJammed(column)) continue;

		if (isFull(column)) {
			setDeployColumn(column);
			return column;
		}
	}

	for (int column = NARCO_FIRST_COLUMN; column < currentColumn; ++column) {
		if (isJammed(column)) continue;

		if (isFull(column)) {
			setDeployColumn(column);
			return column;
		}
	}

	return 0;
}

void Narco::setDeployColumn(uint8_t column)
{
	uint32_t c = column;
	m_narco |= (c << 18);
}

const char * Narco::getInfoText()
{
	updateWithJam();

	for (int column = 1; column <= 9; ++column) {
		if (isFull(column))
			m_infoText[14 - column] = '#';
		if (isJammed(column))
			m_infoText[14 - column] = 'J';
	}

	return &m_infoText[0];
}

