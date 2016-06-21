#include <RCSwitch.h>
#define VIBRO_PIN 6
#define VIBRO_ON 2
#define VIBRO_OFF 8
RCSwitch receiver = RCSwitch(); 


void setup() {
  pinMode(VIBRO_PIN, OUTPUT);
  Serial.begin(115200);
  receiver.enableReceive(0); //PIN 2 for wiring
}

void loop() {
  if(receiver.available()){
    debug();
    if(receiver.getReceivedValue() == VIBRO_ON){
      while(receiver.getReceivedBitlength() != VIBRO_OFF){
        debug();
        Serial.println("Vibro on");
        digitalWrite(VIBRO_PIN, HIGH);
        delay(800);
        Serial.println("Vibro off");
        digitalWrite(VIBRO_PIN, LOW);
        delay(400);
        receiver.resetAvailable();
      }
    }  
  } else {
    Serial.println("Receiver unavailable.");  
    delay(200);
  }
}

void debug() {
   Serial.print("Receiving: "); 
   Serial.print( receiver.getReceivedValue() );
      
   Serial.print(" / ");
      
   Serial.print( receiver.getReceivedBitlength() );
   Serial.print("bit ");
      
   Serial.print("protocol:: ");
   Serial.println( receiver.getReceivedProtocol() );  
}
