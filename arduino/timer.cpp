// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include "Arduino.h"
#include "timer.h"

Timer::Timer() :
	m_startTime(0), m_interval(0), m_isActive(false)
{
}

Timer::Timer(unsigned long interval) :
	m_startTime(0), m_interval(interval), m_isActive(false)
{
}

bool Timer::timedOut()
{
	if (!isActive()) return false;
	
	if (millis() >= (m_startTime + m_interval))
		return true;

	return false;
}

void Timer::start(unsigned long interval)
{
	m_interval = interval;
	start();
}

void Timer::start()
{
	m_startTime = millis();
	m_isActive = true;
}
