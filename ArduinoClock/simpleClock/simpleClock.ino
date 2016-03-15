void setup() {
  // put your setup code here, to run once:
  for(int i = 2; i <= 13; i++){
    set_pin_output(i);  
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 2; i <= 13; i++){
    turn_on_led(i);
    delay(2000);
  }
  for(int i = 2; i <= 13; i++){
    turn_off_led(i);
  }
  delay(2000);
}


// sets a given pin as an output pin
void set_pin_output(int pin) {
  pinMode(pin, OUTPUT);
}

// sets the current for a pin
void turn_on_led(int pin) {
  digitalWrite(pin, HIGH);
}

// sets a pin to no current
void turn_off_led(int pin) {
  digitalWrite(pin, LOW);  
}

