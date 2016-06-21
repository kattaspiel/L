#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define NEOPIN 6

//Initialising the ring
Adafruit_NeoPixel ring = Adafruit_NeoPixel(24, NEOPIN, NEO_GRB + NEO_KHZ800);
//temporary interval at 2 seconds
int interval = 2000;
// needed ring colours
uint32_t blue = ring.Color(0, 64, 64);
uint32_t green = ring.Color(0, 64, 0, 64);
uint32_t off = ring.Color(0 ,0, 0);

void setup() {
  //start up the ring
  ring.begin();
  ring.show();  
}

void loop() {
  //run blue lights to indicate time 
  for(int i = 0; i < ring.numPixels(); i++){
    // set the pixel blue
    ring.setPixelColor(i, blue);
    ring.show();
    // for now 2 seconds
    delay(interval);
  }
  // turn off the colours again
  for(int i = 0; i < ring.numPixels(); i++){
    ring.setPixelColor(i, off);
  }
  ring.show();
  delay(interval);
}
