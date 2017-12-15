/* 
 * Copyright 2017 Alessio Villa
 * 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
#ifndef TELNETC_H
#define TELNETC_H

#include <Ethernet.h>

//#define TNDBG 1

#ifdef TNDBG
 #define DEBUG_PRINT(x)  Serial.println (x)
#else
 #define DEBUG_PRINT(x)
#endif

const uint8_t MAX_OUT_BUFFER_LENGTH = 64;
const unsigned int LISTEN_TOUT = 15000;
 
class telnetClient{

public:

    telnetClient(EthernetClient& client);

    bool login(IPAddress serverIpAddress, const char* username, const char* password, uint8_t port = 23);
	bool sendCommand(const char* cmd);
    void disconnect();

private:
	
	EthernetClient* client;
	
	bool send(const char* buf, bool checkEcho);
	void receive();
	void parse();
	bool listenUntil(char c1, char c2 = 0, char c3 = 0, char c4 = 0);
	void print(char c);
};

#endif
