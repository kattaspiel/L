#include "pitches.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN 6


Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, PIN, NEO_GRB + NEO_KHZ800);
int buttonState = 0;
long currentMillis = 0;
int interval = 4000;
int alarm = 0;
int timey = 0;
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void setup() {
  pinMode(A0, INPUT);
  strip.begin();
  strip.show();
}

void loop() {
  // put your main code here, to run repeatedly:
  if( timey < strip.numPixels()){
    if (timey > 0 && alarm > 0 && timey+1 == alarm){
      play_sounds();
    }
    strip.setPixelColor(timey, strip.Color(226,0,86));
    strip.show();
    timey++;
    check_input();
  }
  else{
    for (int j; j < strip.numPixels(); j++){
      strip.setPixelColor(j, strip.Color(0,0,0));
      strip.show();
      delay(100);
    }
    timey = 0;
  }
  check_input();
}

// checks the input in certain intervals
void check_input() {
  currentMillis = millis();
  while(millis() - currentMillis <= interval) {
    read_button(); 
  }  
}

// to have input going during delay
void read_button(){
  buttonState = digitalRead(A0);
  if(buttonState == HIGH) {
    strip.setPixelColor(alarm, strip.Color(238,127,0));
    if (alarm > timey+1){
      strip.setPixelColor(alarm-1, strip.Color(0,0,0));
    }
    else{
      strip.setPixelColor(alarm-1, strip.Color(226,0,86));  
    }
    strip.show();
    if (alarm >= strip.numPixels()) {
      alarm = 0;  
    } else {
      alarm++;  
    }
  }
  delay(150);
}

void play_sounds(){
  for(int note = 0; note < 8; note++){
    int duration = 1000 / noteDurations[note];
    tone(8, melody[note], duration);
    int pause = duration * 1.3;
    delay(pause);
    noTone(8);
  }  
  alarm = 0;
}
