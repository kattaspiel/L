#include <RCSwitch.h>
#define VIBRO_PIN 9
RCSwitch receiver = RCSwitch(); 


void setup()
{
  pinMode(VIBRO_PIN, OUTPUT);
  Serial.begin(9600); 
  receiver.enableReceive(0); //PIN 2 on the board equals interrupt 0
}

void loop()
{
  //while(true){}
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
      while(receiver.getReceivedValue() != 65){
        Serial.println("in loop");
        Serial.println(receiver.getReceivedValue());
        digitalWrite(VIBRO_PIN, HIGH);
        delay(800);
        digitalWrite(VIBRO_PIN, LOW);
        delay(400);
        receiver.resetAvailable();
      }
    }
    receiver.resetAvailable(); // reset for next signal
  }
}


