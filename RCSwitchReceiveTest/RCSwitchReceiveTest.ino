#include <RCSwitch.h>
RCSwitch receiver = RCSwitch(); 

void setup()
{
  Serial.begin(9600); 
  receiver.enableReceive(0); //PIN 2 on the board equals interrupt 0
}

void loop()
{
  // in case you receive data
  if (receiver.available()) 
  {    
    int value = receiver.getReceivedValue(); 
    
    if (value == 0) 
    {
      Serial.print("Unknown Code"); 
    }
    else
    {
      Serial.print("Receiving: "); 
      Serial.print( receiver.getReceivedValue() );
      
      Serial.print(" / ");
      
      Serial.print( receiver.getReceivedBitlength() );
      Serial.print("bit ");
      
      Serial.print("protocol:: ");
      Serial.println( receiver.getReceivedProtocol() );
    }
    receiver.resetAvailable(); // reset for next signal
  }
}


