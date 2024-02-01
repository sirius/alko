// SPDX-FileCopyrightText: 2024 Sirius Bakke <sirius@bakke.co>
// SPDX-License-Identifier: Apache-2.0

#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <stdint.h>
#include <stddef.h>

class Datagram
{
	friend class Network;

public:
	explicit Datagram(uint8_t dataSize = 0);
	Datagram(const Datagram& dgr);
	Datagram& operator= (const Datagram& dgr);
	~Datagram();
	const char getCommand() const;
	void setCommand(const char command);
	const char* getData() const;
	void setData(const char* data, uint8_t dataSize = 0);
	uint8_t getDataSize() const;
	bool setDataSize(uint8_t);
	uint16_t getIdstate() const;
	void setId(uint16_t id);
	bool checksumIsValid() const;
	void setChecksum(uint16_t sum);
	void updateChecksum();

private:
	bool isValid() const { return (NULL != m_dptr); }
	uint16_t generateChecksum() const;
	uint8_t* m_dptr;
	char * getDataPtr(); 
	const uint8_t * getDatagramPtr();
};

#endif // DATAGRAM_H
