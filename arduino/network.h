#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <Arduino.h>
#include <datagram.h>

class Network
{
public:
	Network();
	void begin();
	bool datagramIsPending();
	const Datagram getPendingDatagram();
	void sendDatagram(Datagram dgr);
	void sendCommand(const char command, uint16_t id = 0);

private:
	bool handleDatagram(Datagram dgr);
	int sizePending;
	Datagram* m_datagram;
};

#endif // NETWORK_H
