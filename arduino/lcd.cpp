// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include <LiquidCrystal.h>
#include <Wire.h>
#include "lcd.h"
#include "config.h"

LCD::LCD(uint8_t i2cAddr) : 
  LiquidCrystal(i2cAddr)
{
	clear();
}

void LCD::clear()
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 20; ++j)
			m_text[i][j] = ' ';
		m_text[i][20] = '\0';
	}
}

void LCD::reset()
{
    begin(LCD_COLS, LCD_ROWS);
    addNorwegianChars();
}

void LCD::setText(int line, const char* text, int length)
{
        if (NULL == text) return;
        if (length <= 0) return;
        if (length > 20) length = 20;

        char* dst = &m_text[line % 4][0];

        while (*text && length--)
                *dst++ = *text++;

		char* end = &m_text[line % 4][20];

		while (dst < end)
			*dst++ = ' ';

		*end = '\0';

		norwegianChars(line);
}

void LCD::update()
{
	for (int i = 0; i < 4; ++i) {
		setCursor(0, i);
		print(m_text[i]);
	}
}

void LCD::norwegianChars(int line)
{
	line = line % 4;
	for (int i = 0; i < 20; ++i) {
		switch (m_text[line][i]) {
		case 230: //æ
			m_text[line][i] = 1;
			break;
		case 198: //Æ
			m_text[line][i] = 2;
			break;
		case 248: //ø
			m_text[line][i] = 3;
			break;
		case 216: //Ø
			m_text[line][i] = 4;
			break;
		case 229: //å
			m_text[line][i] = 5;
			break;
		case 197: //Å
			m_text[line][i] = 6;
			break;
		default:
			break;
		}	
	}
}

void LCD::setColor(uint8_t r, uint8_t g, uint8_t b) {
	Wire.beginTransmission(0x09);
	Wire.write('o');
	Wire.write('n');
	Wire.write(r);
	Wire.write(g);
	Wire.write(b);
	Wire.endTransmission();
}

void LCD::setColorCycle() {
	Wire.beginTransmission(0x09);
	Wire.write('o');
	Wire.write('p');
	Wire.write(10);
	Wire.write(0);
	Wire.write(0);
	Wire.endTransmission();
}

void LCD::setRedCycle() {
	Wire.beginTransmission(0x09);
	Wire.write('o');
	Wire.write('n');
	Wire.write(0xFF);
	Wire.write(0);
	Wire.write(0);
	Wire.write('p');
	Wire.write(3);
	Wire.write(0);
	Wire.write(0);
	Wire.endTransmission();
}

void LCD::addNorwegianChars()
{
	uint8_t _char1[] = {0x0,0x0,0x1b,0x5,0x1f,0x14,0x1f,0x0}; //æ
	uint8_t _char2[] = {0xf,0x14,0x14,0x1f,0x14,0x14,0x17,0x0}; //Æ
	uint8_t _char3[] = {0x0,0x0,0xe,0x13,0x15,0x19,0xe,0x0}; //ø
	uint8_t _char4[] = {0xe,0x13,0x15,0x15,0x15,0x19,0xe,0x0}; //Ø
	uint8_t _char5[] = {0xe,0xa,0xe,0x1,0xf,0x11,0xf,0x0}; //å
	uint8_t _char6[] = {0xe,0xa,0x0e,0x11,0x1f,0x11,0x11,0x0}; //Å
	createChar(1, _char1);
	createChar(2, _char2);
	createChar(3, _char3);
	createChar(4, _char4);
	createChar(5, _char5);
	createChar(6, _char6);
}
