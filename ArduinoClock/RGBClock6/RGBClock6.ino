int alarmLED = 0;
int buttonState = 0;
long currentMillis = 0;
int interval = 4000;

void setup() {
  // setting all output pins
  for(int i = 2; i <= 13; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
  pinMode(A0, INPUT);
  
}

void loop() {
  // looping over all lights in red
  for(int i = 2; i <= 13; i++) {
    if(i%2 == 1) {
      digitalWrite(i, LOW);
      check_input();
    }  
  }
  // turn all lights off
  for(int i = 2; i <= 13; i++) {
    digitalWrite(i, HIGH); 
    alarmLED = 0; 
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

// to have input going doring delay
void read_button(){
  buttonState = digitalRead(A0);
  if(buttonState == HIGH) {
    if(alarmLED >= 2){
      digitalWrite(alarmLED, HIGH);
    }
    alarmLED = alarmLED+2;
    if(alarmLED >= 13) {
      alarmLED = 0;  
    }
    if(alarmLED >= 2) {
      digitalWrite(alarmLED, LOW);
    }
  }
  delay(150);
}
