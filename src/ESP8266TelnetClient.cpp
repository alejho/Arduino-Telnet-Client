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
 
#include "ESP8266telnetClient.h"


ESP8266telnetClient::ESP8266telnetClient(WiFiClient& client){
	this->client = &client;	
}

bool ESP8266telnetClient::login(IPAddress serverIpAddress, const char* username, const char* password, uint8_t port){
	
	this->print('\n');
	this->print('\r');
	
	DEBUG_PRINT(F("login|connecting..."));
	if(this->client->connect(serverIpAddress, port)){
		DEBUG_PRINT(F("login|connected!"));
		//here there will be the initial negotiation
		if(!listenUntil(':')){
			return false;
		}
		//listen();
		DEBUG_PRINT(F("login|sending username"));
		if (!this->send(username, false)) return false; 
		listenUntil(':');
		DEBUG_PRINT(F("login|sending password"));
		if (!this->send(password, false)) return false;
		
		#ifdef MT_VM
		//mikrotik router with demo license
		this->listenUntil('!');
		this->send("", false);
		#endif
		
		return this->waitPrompt();
		//listen();
		//return true;
		
		
	}
	else{
		DEBUG_PRINT(F("login|connection failed!"));
		return false;
	}
}

bool ESP8266telnetClient::sendCommand(const char* cmd){
	
	this->send(cmd);
	//negotiation until the server show the command prompt again
	if (strcmp(cmd, "exit") != 0){
		return this->waitPrompt();
	}
	else{
		this->disconnect();
	}
	
}

void ESP8266telnetClient::disconnect(){
	this->client->stop();
}

bool ESP8266telnetClient::send(const char* buf, bool waitEcho){
	
	uint8_t l_size = strnlen(buf, MAX_OUT_BUFFER_LENGTH);
	if(l_size == MAX_OUT_BUFFER_LENGTH){
		DEBUG_PRINT(F("send|BAD INPUT"));
		return false;
	}
	
	char l_outBuffer[MAX_OUT_BUFFER_LENGTH];
	strlcpy(l_outBuffer, buf, MAX_OUT_BUFFER_LENGTH);
	if(strlcat(l_outBuffer, "\r\n", MAX_OUT_BUFFER_LENGTH) >= MAX_OUT_BUFFER_LENGTH){
		DEBUG_PRINT(F("send|BAD INPUT"));
		return false;
	}
	
	l_size = strnlen(l_outBuffer, MAX_OUT_BUFFER_LENGTH);
	for (uint8_t i = 0; i < l_size; ++i){
		if(l_outBuffer[i] > 0){
			this->client->write(l_outBuffer[i]);
			this->print(l_outBuffer[i]);
			if (waitEcho){
				while (this->client->available () == 0) delay (1);
				char inByte = this->client->read();
			}
		}
	}
		 
	//this->print('\r');
	return true;
}

void ESP8266telnetClient::negotiate(){
	
	byte verb, opt;    
	byte outBuf[3] = {255, 0, 0};
	
	DEBUG_PRINT(F("negotiate|server:IAC"));
	verb = this->client->read ();                        
	if (verb == - 1) return;                       
	switch (verb) {                               
		case 255:                    
			//...no it isn't!
			DEBUG_PRINT(F("negotiate|server:IAC escape"));
			this->print(char (verb));
		break;                                    
		case 251:
		  //to a WILL statement... 
		  DEBUG_PRINT(F("negotiate|server:WILL"));
		  opt = this->client->read();
		  if (opt == -1) break;
		  DEBUG_PRINT(F("negotiate|server opt: "));
		  DEBUG_PRINT(opt);
		  //always answer DO!
		  outBuf[1] = 253;
		  outBuf[2] = opt;
		  this->client->write(outBuf, 3);
		  this->client->flush();
		  DEBUG_PRINT(F("negotiate|client:IAC"));
		  DEBUG_PRINT(F("negotiate|client:DO"));
		break;
		case 252:                                 
		  DEBUG_PRINT(F("negotiate|server:WONT"));
		break;
		case 253:              
		  //to a DO request...		
		  DEBUG_PRINT(F("negotiate|server:DO"));
		  opt = this->client->read();
		  if (opt == -1) break;
		  DEBUG_PRINT(F("negotiate|server opt: "));
		  DEBUG_PRINT(opt);
		  //alway answer WONT!
		  outBuf[1] = 252;
		  outBuf[2] = opt;
		  this->client->write(outBuf, 3);
		  this->client->flush();
		  DEBUG_PRINT(F("negotiate|client:IAC"));
		  DEBUG_PRINT(F("negotiate|client:WONT"));
		break;
		case 254:                                 
		  DEBUG_PRINT(F("negotiate|server:DONT"));     
		break;                                    
	}
			
}

void ESP8266telnetClient::listen(){
	
	while (this->client->available() == 0) delay (1);   
	
	byte inByte;
	unsigned long startMillis = millis();
	
	while(1){
		if (client->available() > 0){
			startMillis = millis();
			inByte = this->client->read ();
			if (inByte <= 0){
				//DEBUG_PRINT(F("listen|what?"));
			}
			else if(inByte == 255){
				this->negotiate();
			}
			else{
				//is stuff to be displayed
				this->print(char(inByte));
			}
		}
		else if (millis() - startMillis > LISTEN_TOUT){
			DEBUG_PRINT(F("listen|TIMEOUT!!!"));     
			return;
		}
	}				
}	

bool ESP8266telnetClient::listenUntil(char c){
	
	byte inByte;
	unsigned long startMillis = millis();
	//listen incoming bytes untile one char in the array arrive
	while (this->client->available() == 0) delay (1);   
	startMillis = millis();
	do {                            
		if(this->client->available() > 0){
			inByte = this->client->read();
			if (inByte <= 0){
				//DEBUG_PRINT(F("listen|what?"));
			}
			else if(inByte == 255){
				this->negotiate();
			}
			else{
				//is stuff to be displayed
				this->print(char(inByte));
			}
			if (char(inByte) == c){
				DEBUG_PRINT(F("listenUntil|TERMINATOR RECEIVED"));
				return true;
			}
		}
		else if (millis() - startMillis > LISTEN_TOUT){
			DEBUG_PRINT(F("listen|TIMEOUT!!!"));     
			return false;
		}                   
	}while (1);
	
}

bool ESP8266telnetClient::waitPrompt(){
	
	bool l_bLoop = false;
	unsigned long startMillis = millis();
	
	do
	{
		if (!this->listenUntil(m_promptChar)) return false;
		char l_lastByte = this->client->read();
		do
		{
			l_bLoop = this->client->available() > 0;
			if (l_bLoop){
				DEBUG_PRINT(F("waitPrompt|FALSE PROMPT DETECTED"));
				this->print('\r');
				//this->print('\n');
				break;
			}
		}while(millis()-startMillis < PROMPT_REC_TOUT);
		
	}while(l_bLoop);
	
	//this->print('\n');
	//this->print('\r');
	DEBUG_PRINT(F("waitPrompt|END"));
	return true;
}

void ESP8266telnetClient::print(char c){
	//edit this function if you want a different output!
	Serial.print(c);
}

void ESP8266telnetClient::setPromptChar(char c){
	m_promptChar = c;
}


