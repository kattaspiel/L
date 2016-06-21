#include <RCSwitch.h>  
RCSwitch sender = RCSwitch(); 
int sendPIN = 2;

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

  delay(8000);
  
  sender.send(0101, 8); // send 0101 with a length of 4 bit
  delay(4000);

  delay(8000);

  
}
