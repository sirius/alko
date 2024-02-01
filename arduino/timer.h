// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef TIMER_H
#define TIMER_H

#include "Arduino.h"

class Timer
{
public:
	Timer();
	Timer(unsigned long interval);
	bool timedOut();
	bool isActive() { return m_isActive; }
	void setInterval(unsigned long interval) { m_interval = interval; }
	void start(unsigned long interval);
	void start();
	void stop() { m_isActive = false; }
private:
	unsigned long m_startTime;
	unsigned long m_interval;
	bool m_isActive;
};

#endif // TIMER_H
