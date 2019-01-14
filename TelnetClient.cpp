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
 
#include "TelnetClient.h"


telnetClient::telnetClient(EthernetClient& client){
	this->client = &client;	
}

bool telnetClient::login(IPAddress serverIpAddress, const char* username, const char* password, uint8_t port){
	
	this->print('\n');
	this->print('\r');
	
	DEBUG_PRINT(F("login|connecting..."));
	if(this->client->connect(serverIpAddress, port)){
		DEBUG_PRINT(F("login|connected!"));
		//negotation
		this->negotiation();
		//we should be asked for the login username at this point...
		DEBUG_PRINT(F("login|sending username"));
		if (!this->send(username, true)) return false; 
		//negotiation until the server show the password prompt
		if (!this->listenUntil(':')) return false;
		DEBUG_PRINT(F("login|sending password"));
		if (!this->send(password, false)) return false;
		//negotiation until the server show the command prompt or timeout
		
		//mikrotik router with demo license
		//this->listenUntil('!');
		//emptyInputBuffer(true);
		//this->send("", false);
		////////////////////////////
		
		return this->waitPrompt();
	}
	else{
		DEBUG_PRINT(F("login|connection failed!"));
		return false;
	}
}

bool telnetClient::sendCommand(const char* cmd){
	
	this->send(cmd, true);
	//negotiation until the server show the command prompt again
	if (strcmp(cmd, "exit") != 0){
		return this->waitPrompt();
	}
	
}

void telnetClient::disconnect(){
	this->client->stop();
}

bool telnetClient::send(const char* buf, bool checkEcho){
	
	uint8_t l_size = strnlen(buf, MAX_OUT_BUFFER_LENGTH);
	if(l_size == MAX_OUT_BUFFER_LENGTH){
		DEBUG_PRINT(F("send|BAD INPUT"));
		return false;
	}
	
	//emptyInputBuffer(true);
	
	char l_outBuffer[MAX_OUT_BUFFER_LENGTH];
	strlcpy(l_outBuffer, buf, MAX_OUT_BUFFER_LENGTH);
	if(strlcat(l_outBuffer, "\r\n", MAX_OUT_BUFFER_LENGTH) >= MAX_OUT_BUFFER_LENGTH){
		DEBUG_PRINT(F("send|BAD INPUT"));
		return false;
	}
	if(checkEcho){
		l_size = strnlen(l_outBuffer, MAX_OUT_BUFFER_LENGTH);
		for (uint8_t i = 0; i < l_size; ++i){
			if(l_outBuffer[i] > 0){
				this->client->write(l_outBuffer[i]);
				this->print(l_outBuffer[i]);
				while (this->client->available () == 0) delay (1);
				byte inByte = this->client->read();
				if((char)inByte != l_outBuffer[i]){
				DEBUG_PRINT(F("send|WRONG ECHO FROM SERVER"));
				return false;
			}
			}
		} 
	}
	else{
		this->client->write(l_outBuffer);
	}
	this->client->flush();
	this->print('\r');
	return true;
}

void telnetClient::negotiation(){
	
	while (this->client->available() == 0) delay (1);        
	do {                                               
		this->negotiate();                                  
		delay(NEGOTIATION_DELAY);                                     
	}
	while(this->client->available() > 0);

}

void telnetClient::negotiate(){
	
	int inByte, verb, opt;    
	
	while (this->client->available () > 0) {                    
		inByte = this->client->read ();                         
		switch (inByte) {                                     
			case -1:        
				DEBUG_PRINT(F("negotiate|what?"));
				break;                                            
			case 255:
				//is stuff to negotiate with the server
				DEBUG_PRINT(F("negotiate|server:IAC"));
				verb = this->client->read ();                        
				if (verb == - 1) break;                       
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
					  DEBUG_PRINT(F("negotiate|client:IAC"));
					  this->client->write (255);
					  DEBUG_PRINT(F("negotiate|client:DO"));
					  this->client->write (253);
					  this->client->write(opt);
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
					  //always answer WONT!
					  this->client->write(255);
					  DEBUG_PRINT(F("negotiate|client:IAC"));
					  this->client->write(252);
					  DEBUG_PRINT(F("negotiate|client:WONT"));
					  this->client->write(opt);
					break;
					case 254:                                 
					  DEBUG_PRINT(F("negotiate|server:DONT"));     
					break;                                    
				}
			break;                                            
			default:                                          
				//is stuff to be displayed
				this->print(char(inByte));
			break;                                            
		}
	}
}

bool telnetClient::listenUntil(char c){

	char l_cArr[1] = {c};
	return listenUntil(l_cArr, 1);
}

bool telnetClient::listenUntil(char* c, uint8_t len){
	
	//listen incoming bytes untile one of the chars in the array arrive
	
	char l_lastByte = 0;
	unsigned long startMillis = millis();
	
	while (this->client->available() == 0) delay (1);   
	bool l_bTerminator = false;
	
	do {                            
		if(this->client->available() > 0){
			l_lastByte = this->client->read();
			if(l_lastByte > 0)this->print(l_lastByte);                                     
			
			for (uint8_t i = 0; i < len; ++i){
				if (l_lastByte == c[i]){
					DEBUG_PRINT(F("listenUntil|TERMINATOR RECEIVED"));
					l_bTerminator = true;
					break;
				}
			}
		}                   
	}while (!l_bTerminator && this->client->connected() && (millis()-startMillis < LISTEN_TOUT));
	
	if(millis()-startMillis >= LISTEN_TOUT){
		DEBUG_PRINT(F("listenUntil|TIMEOUT OCCURED"));
		return false;
	}
	
	return true;
}

bool telnetClient::waitPrompt(){
	
	bool l_bLoop = false;
	unsigned long startMillis = millis();
	
	do
	{
		if (!this->listenUntil(PROMPT_CHARS, ARRAYSIZE(PROMPT_CHARS))) return false;
		char l_lastByte = this->client->read();
		do
		{
			l_bLoop = this->client->available() > 0;
			if (l_bLoop){
				DEBUG_PRINT(F("waitPrompt|FALSE PROMPT DETECTED"));
				break;
			}
			
		}while(millis()-startMillis < PROMPT_REC_TOUT);
		
	}while(l_bLoop);
	
	return true;
}

void telnetClient::print(char c){
	//edit this function if you want a different output!
	Serial.print(c);
}

void telnetClient::emptyInputBuffer(bool printIt){
	
	char l_lastByte;
	//empty the input buffer
	while (this->client->available() > 0){
		l_lastByte = this->client->read();
		if (printIt) this->print(l_lastByte);
	}
}

