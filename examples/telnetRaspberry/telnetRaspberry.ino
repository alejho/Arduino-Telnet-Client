#include <TelnetClient.h>
#include <SPI.h>
#include <Ethernet.h>


EthernetClient client;                                   
telnetClient tc(client); 

byte clientMAC[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 
IPAddress clientIp (192, 168, 1, 177); 

//put here your raspberry's ip address
IPAddress raspberryIp (192, 168, 1, 11);    

void setup () { 
                                       
  Serial.begin (9600);                                 
  Ethernet.begin (clientMAC, clientIp);   
  
  //want to use dhcp?
  //if (!Ethernet.begin (clientMAC)){
       //Serial.println("\r\nDHCP error");
       //while(1);
  //}

  //WICH CHARACTER SHOULD BE INTERPRETED AS "PROMPT"?
  tc.setPromptChar('$');

  //this is to trigger manually the login 
  //since it could be a problem to attach the serial monitor while negotiating with the server (it cause the board reset)
  //remove it or replace it with a delay/wait of a digital input in case you're not using the serial monitor
  char key = 0;
  Serial.println("\r\npress Enter to begin:");
  do{
    key = Serial.read();
  }while(key<=0);
  
  //PUT HERE YOUR USERNAME/PASSWORD
  if(tc.login(raspberryIp, "pi", "yourpassword")){        //tc.login(raspberryIp, "pi", "raspberry", 1234) if you want to specify a port different than 23
    tc.sendCommand("ls -l");
    tc.sendCommand("ifconfig");
         
  }
  else{
    Serial.println("login failed");
  }
  tc.disconnect();
}

void loop () {                                         //  run your loop routine
}
