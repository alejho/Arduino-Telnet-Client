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
 * You should have negotiationd a copy of the GNU General Public License
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

#define ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))
const uint8_t NEGOTIATION_DELAY = 100;

////////////////CONFIGURATION////////////////////////////////////////////////////////////////////////////////////////////////////

//how long the command sent may be long
const uint8_t MAX_OUT_BUFFER_LENGTH = 150;
//how long you'll wait for an expected answer from the server
const unsigned int LISTEN_TOUT = 20000;
//wich characters should be interpreted as "prompt"
const char PROMPT_CHARS[] = {'$', '>', '#'}; 
//how long, after a "prompt char" is received you can confirm it's the real prompt and not just part of the server's answer
const uint16_t PROMPT_REC_TOUT = 300;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
class telnetClient{

public:

    telnetClient(EthernetClient& client);

    bool login(IPAddress serverIpAddress, const char* username, const char* password, uint8_t port = 23);
	bool sendCommand(const char* cmd);
    void disconnect();

private:
	
	EthernetClient* client;
	
	bool send(const char* buf, bool checkEcho);
	void negotiation();
	void negotiate();
	bool listenUntil(char* c, uint8_t len);
	bool listenUntil(char c);
	bool waitPrompt();
	void print(char c);
	void emptyInputBuffer(bool printIt);
};

#endif
