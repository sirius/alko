// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include <Arduino.h>
#include <Wire.h>
#include <avr/wdt.h>
#include "dixie.h"
#include "config.h"

Dixie dixie;
const int serviceButtonPin = 2;
const int ledPin = 13; // DEBUG
int ledState = HIGH; // DEBUG
int serviceButtonState;
int lastServiceButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = FORCE_SERVICE_TIME;
int reading = HIGH;

void setup()
{
    wdt_disable();
    pinMode(serviceButtonPin, INPUT);
    digitalWrite(serviceButtonPin, HIGH);
    pinMode(ledPin, OUTPUT); // DEBUG
    digitalWrite(ledPin, ledState); // DEBUG

    dixie.begin();
    wdt_enable(WDTO_8S);
}

void loop()
{
    dixie.update();

    wdt_reset();

    reading = digitalRead(serviceButtonPin);
    if (reading != lastServiceButtonState) {
        lastDebounceTime = millis();
    }

    if  ((millis() - lastDebounceTime) > debounceDelay) {
        serviceButtonState = reading;
        if (serviceButtonState == LOW) {
            ledState = !ledState; // DEBUG
            digitalWrite(ledPin, ledState); // DEBUG
	        dixie.forceServiceState();
            lastDebounceTime = millis();
            reading = HIGH;
        } else {
            reading = HIGH;
        }
    }
    lastServiceButtonState = reading;
}
