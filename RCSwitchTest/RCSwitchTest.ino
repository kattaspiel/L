#include <RCSwitch.h>  
RCSwitch sender = RCSwitch(); 
int sendPIN = 10;

void setup()
{
  Serial.begin(9600); 
  sender.enableTransmit(sendPIN);
  sender.setProtocol(2);
}


void loop()
{
  sender.send(1010, 4); //send 1010 with a length of 4 bit
  delay(4000); // wait 4 seconds
}
