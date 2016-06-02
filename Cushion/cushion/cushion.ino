#define VIBRO_PIN 9
#define BUTTON 4

int buttonState = 0;

void setup() {
  pinMode(VIBRO_PIN, OUTPUT);
  pinMode(BUTTON, INPUT);
}

void loop() {
  buttonState = digitalRead(BUTTON);
  if (buttonState == HIGH) {
    digitalWrite(VIBRO_PIN, HIGH);  
  } else {
    digitalWrite(VIBRO_PIN, LOW);    
  }
}
