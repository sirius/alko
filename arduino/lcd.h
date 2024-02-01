// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include <LiquidCrystal.h>

const int LCDRedPin = 3;
const int LCDGreenPin = 12;
const int LCDBluePin = 13;

class LCD : public LiquidCrystal
{
public:
  LCD(uint8_t i2cAddr);
	void update();
    void reset();
	void clear();
	void setText(int line, const char* text = "", int length = 20);
	void setColor(uint8_t r, uint8_t g, uint8_t b);
	void setColorCycle();
	void setRedCycle();
	void addNorwegianChars();
private:
	char m_text[4][21];
	void norwegianChars(int line);
};

#endif // LCD_H
