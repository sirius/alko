// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#include <stdlib.h>
#include <stddef.h>
#include "datagram.h"

Datagram::Datagram(uint8_t dataSize) :
	m_dptr(NULL)
{
	if (dataSize < 0)
		dataSize = 0;

	m_dptr = (uint8_t*) malloc((1 + 8 + dataSize) * sizeof(uint8_t)); // Ref counter + header + data
	m_dptr[0] = 0x01; // Ref counter
	m_dptr[1] = 0x42; // Signature byte
	m_dptr[2] = 0x53; // Signature byte
	m_dptr[3] = (8 + dataSize); // Header + data size
	m_dptr[4] = 0; // Id, high byte
	m_dptr[5] = 0; // Id, low byte
	m_dptr[6] = 0; // Checksum, high byte
	m_dptr[7] = 0; // Checksum, low byte
	m_dptr[8] = 0; // Command
    // Optionally: data in m_dptr[9]
}

Datagram::Datagram(const Datagram &dgr)
{
	m_dptr = dgr.m_dptr;
	m_dptr[0]++; // Inc ref counter
}

Datagram& Datagram::operator= (const Datagram& dgr)
{
	if (&dgr == this) return *this;

	this->~Datagram();

	m_dptr = dgr.m_dptr;
	m_dptr[0]++; // Inc ref counter

	return *this;
}

Datagram::~Datagram()
{
	if (isValid()) {
		if (1 == m_dptr[0])
			free(m_dptr);
		else
			m_dptr[0]--;
	}
}

const char Datagram::getCommand() const
{
	if (!isValid()) return 0;

	return m_dptr[8];
}

void Datagram::setCommand(const char command)
{
	if (!isValid()) return;

	m_dptr[8] = command;
}

const char* Datagram::getData() const
{
	if (!isValid()) return 0;

    // TODO: bounds check
	if (getDataSize() > 0)
		return (char *) &m_dptr[9];
    return NULL;
}

void Datagram::setData(const char* data, uint8_t dataSize)
{
	if (0 == dataSize) dataSize = getDataSize();

	uint8_t bufferDataSize = getDataSize();
	if (dataSize > bufferDataSize)
		dataSize = bufferDataSize;	

	char* dst = (char *) &m_dptr[9];
	while (dataSize--)
		*dst++ = *data++;
}

uint8_t Datagram::getDataSize() const
{
	if (!isValid()) return 0;

	return (m_dptr[3] - 8);
}

bool Datagram::setDataSize(uint8_t dataSize)
{
	uint8_t* newBuffer = (uint8_t*) realloc(m_dptr, (1 + 8 + dataSize) * sizeof(uint8_t)); // Ref counter + header + data
	if (0 == newBuffer) return false;

	m_dptr[3] = (8 + dataSize); // Don't count ref counter byte
	return true;
}

uint16_t Datagram::getId() const
{
	if (!isValid()) return 0;
	
	uint16_t id = m_dptr[5];
	id |= (m_dptr[4] << 8);

	return id;
}

void Datagram::setId(uint16_t id) 
{
	if (!isValid()) return;

	m_dptr[4] = (id >> 8);
	m_dptr[5] = (id & 0x00FF);
}

bool Datagram::checksumIsValid() const
{
	if (!isValid()) return false;

	return (0 == generateChecksum()); 
}

void Datagram::setChecksum(uint16_t sum)
{
	if (!isValid()) return;

	m_dptr[6] = (sum >> 8);
	m_dptr[7] = (sum & 0x00FF);
}

void Datagram::updateChecksum()
{
	if (!isValid()) return;

	m_dptr[6] = 0;
	m_dptr[7] = 0;
	uint16_t checksum = generateChecksum();
	m_dptr[6] = (checksum >> 8);
	m_dptr[7] = (checksum & 0x00FF);
}

uint16_t Datagram::generateChecksum() const
{
	if (!isValid()) return 0;

	uint16_t* buffer = (uint16_t *) &m_dptr[1]; // First header byte
	uint16_t length = (8 + getDataSize());
	uint32_t sum = 0;
  
	while (length > 1) {
		sum += *buffer++;
		length -= 2;
	}
  
	if (length > 0)
		sum += *(uint8_t *)buffer;
  
	while (sum>>16)
		sum = (sum & 0xffff) + (sum >> 16);
  
  return (uint16_t)(~sum);
}

char * Datagram::getDataPtr()
{
	if (!isValid()) return NULL;

    // TODO: bounds check
	return (char *) &m_dptr[9];
}

const uint8_t * Datagram::getDatagramPtr()
{
	if (!isValid()) return NULL;

	return &m_dptr[1];
}
