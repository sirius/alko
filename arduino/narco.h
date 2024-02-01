// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef NARCO_H
#define NARCO_H

#include <stdint.h>
#include <Adafruit_MCP23017.h>

#define NARCO_FIRST_COLUMN 1
#define NARCO_LAST_COLUMN 9

class Narco
{
public:
	Narco();
	void begin();
	void update();
	void updateWithJam();
	bool isFull(int column);
	void setFull(int column, bool b);
	bool isJammed(int column);
	void setJammed(int column, bool b);
	bool isHome(int column);
	void powerOn(int column);
	void powerOff();
	void homeAllMotors();
	uint8_t getDeployColumn();
	void setDeployColumn(uint8_t);
	const char * getInfoText();
    uint32_t getRawData() { return m_narco; }

private:
	Adafruit_MCP23017 m_mcp0;
	Adafruit_MCP23017 m_mcp1;
	volatile uint32_t m_narco;
	char m_infoText[16];
};

#endif // NARCO_H
