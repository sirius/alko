// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include <stddef.h>
#include <Arduino.h>
#include "network.h"
#include "datagram.h"
#include "../common/alkoserialcommands.h"

Network::Network() :
	sizePending(0)
{
}

void Network::begin()
{
	Serial.begin(9600);
}

bool Network::datagramIsPending()
{
	if (0 == sizePending) {
		if (Serial.available() < 8)
			return false;

		// Check datagram header signature
		uint8_t sig;
		sig = Serial.read();
		if (0x42 != sig) return false; // First signature byte
		sig = Serial.read();
		if (0x53 != sig) return false; // Second signature byte
		
		// Get datagram size
		sizePending = Serial.read();
		if (sizePending > 32 || sizePending < 8) {
			sizePending = 0; // Too large or too small datagram
			return false;
		}
		
		sizePending -= 3; // We have already read 3 bytes
	}

	if (Serial.available() < sizePending) return false; // Wait for complete datagram

	Datagram dgr(3 + sizePending - 8); // Size of data, including the 3 read bytes, minus header size
	
	// Set ID
	uint16_t id = Serial.read();
	id <<= 8; // Read byte is high byte
	id += Serial.read(); 
	dgr.setId(id);

	// Set checksum
	uint16_t checksum = Serial.read();
	checksum <<= 8; // Read byte is high byte
	checksum += Serial.read();
	dgr.setChecksum(checksum);

	// Set command
	dgr.setCommand(Serial.read());

	// Set data
	Serial.readBytes(dgr.getDataPtr(), dgr.getDataSize());

	// Reset sizePending
	sizePending = 0;

	// Check validity of datagram
	if (!dgr.checksumIsValid()) {
		sendCommand(Alko::Cmd::CHKSUM_INVALID, dgr.getId());
		return false;	
	}

	// Handle datagram
	return handleDatagram(dgr);
}

const Datagram Network::getPendingDatagram()
{
	if (m_datagram == NULL) return Datagram();

	return Datagram(*m_datagram);
}

bool Network::handleDatagram(Datagram dgr)
{
	if (m_datagram != NULL) delete m_datagram;

	m_datagram = new Datagram(dgr);
	return true;
}

void Network::sendDatagram(Datagram dgr)
{
	dgr.updateChecksum();
	Serial.write(dgr.getDatagramPtr(), dgr.getDataSize() + 8);
}

void Network::sendCommand(const char command, uint16_t id)
{
	Datagram dgr;
	dgr.setId(id);
	dgr.setCommand(command);
	dgr.updateChecksum();
	Serial.write(dgr.getDatagramPtr(), dgr.getDataSize() + 8);
} 
