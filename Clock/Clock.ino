#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <RCSwitch.h>
#include "pitches.h"
#include "Wire.h"

#define BUTTON 3
#define NEOPIN 6
#define SPEAKER 8
#define SENDPIN 10
#define MAT 12
#define DS3231_I2C_ADDRESS 0x68
#define DEBUG 0 //set to 0 to have it work with hours instead of minutes
#define PAUSE 3000

//Initialising the ring
Adafruit_NeoPixel ring = Adafruit_NeoPixel(24, NEOPIN, NEO_GRB + NEO_KHZ800);
int interval = 2000;   // temporary interval at 2 seconds

// needed ring colourss
uint32_t blue = ring.Color(0, 64, 64);
uint32_t green = ring.Color(0, 64, 0, 64);
uint32_t off = ring.Color(0 ,0, 0);

//Initialising RCSwitch
RCSwitch sender = RCSwitch();

long last = 0;
int hour = 24;      // Hour is in a scale of 1-24, with 24 representing midnight.
int alarmHour = -1;  // alarmHour is in the same scale as hour, with 0 representing no alarm.
int buttonLastState = LOW;
int dismissLastState = LOW;
bool melodyPlaying = false;
long ceasedPlaying = 0;
int horoscope[] = { NOTE_A3, NOTE_AS3, NOTE_B3, NOTE_C4, NOTE_C4 };
bool first = true;

void setup() {
  // set up the button
  pinMode(BUTTON, INPUT_PULLUP);
  //digitalWrite(BUTTON, INPUT);
  pinMode(MAT, INPUT_PULLUP);
  //digitalWrite(MAT, HIGH);

  // Colours made less blinding
  blue = ring.Color(0, 0, 64, 64);
  green = ring.Color(0, 64, 0, 64);

  // Interval sped up.
  interval = 1000;

  // set up the RTC
  Wire.begin();

  Serial.begin(9600);
  
  if (DEBUG) {
    last = ( getMinuteFromRTC() % 24 ) + 1;
  } else {
    last = getHourFromRTC();
  }

  hour = last;
  last = last-1;
  
  //start up the ring
  ring.begin();
  ring.show();  

  // set up the RCSwitch sender
  sender.enableTransmit(SENDPIN);
  sender.setProtocol(2);

  randomSeed(analogRead(0));
  
  Serial.println("Setup Complete");
}

/*
 * clearRing - Sets all pixels on the ring back to 'off'
 */
void clearRing() {
  for(int i = 0; i < ring.numPixels(); i++){
    ring.setPixelColor(i, off);
  }
  ring.show();
}

/*
 * displayCurrentTime - Sets up time and alarm pixels for the current state.
 */
void displayCurrentTime() {
  clearRing();
  for(int i = 0; i < hour; i++) {
    ring.setPixelColor(i, blue);
  }
  if(0 < alarmHour) {
    // -1 in order to keep the numbering in sync with hour numbering.
    ring.setPixelColor(alarmHour - 1, green);
  }
  ring.show();
  displayTime();
}

void loop() {
  // Every time interval, update the current time.  Uses a timestamp rather than a delay
  // so that it is still responsive while
  int currentHour;
  if (DEBUG) {
    currentHour = (getMinuteFromRTC() % 24) + 1;
  } else {
     currentHour = getHourFromRTC();
     if (currentHour == 0){
        currentHour = 24;  
     }  
  }
   
  if(currentHour != last) {
    
    last = currentHour;

    Serial.print(last);
    Serial.print("\t");
    Serial.println(hour);

    displayCurrentTime();
    if(alarmHour == hour) {
      startAlarm();
    }

    hour++;
    if(25 <= hour) {
      hour = 1;
    }
  }

  // When the button is pressed, advance the alarm.
  if(buttonPressed()) {
    Serial.print(currentTimestamp());
    Serial.print("\tButton Pressed\t");
    alarmHour++;
    Serial.println(alarmHour);
    if(24 < alarmHour) {
      alarmHour = -1;
    }
    hour = hour - 1;
    displayCurrentTime();
    hour = hour + 1;
  }

  // When the dismiss option is activated, turn off the alarm.
  if(dismissPressed() && melodyPlaying) {
    Serial.println("Turning off alarm.");
    ceasePlaying();
    stopVibration();
  }

  // Move along with the melody.
  if(melodyPlaying) {
    playMelody();
  }

  if (ceasedPlaying > 0 && ((currentTimestamp() - ceasedPlaying) > PAUSE)) {
    setHoroscope();
    playHoroscope();  
  }
  first = false;
}

/*
 * currentTimestamp - Stub function to give a timestamp in milliseconds.
 * 
 * returns a timestamp as a long integer
 */
long currentTimestamp() {
  return millis();
}

/* 
 *  buttonPressed - Checks if the button has been newly pressed.  
 *  Pressing the button returns true, holding it will give true ONCE 
 *  and then after that, false until it has been released and pressed again.
 */
bool buttonPressed() {
  int buttonState = digitalRead(BUTTON);
  if(HIGH == buttonState && LOW == buttonLastState) {
    buttonLastState = buttonState;
    return true;
  }
  buttonLastState = buttonState;
  return false;
}

/* 
 *  Checks that the dismiss option has been pressed, and when you returns true.
 */
bool dismissPressed() {
  int buttonState = digitalRead(MAT);
  if(HIGH == buttonState && LOW == dismissLastState) {
    dismissLastState = buttonState;
    return true;
  }
  dismissLastState = buttonState;
  return false;
}

void startVibration() {
  sender.send(1010,4);  
}

void stopVibration() {
  sender.send(0101,8);  
}

void startAlarm() {
  Serial.println("ALARM ALARM ALARM ALARM ALARM");
  playMelody();
  startVibration();
}

// Is a song of some sort.  Because Arduino won't let me use tuples or anything 
// that looks like a tuple, there are two arrays of identical length that have 
// to be kept in sync.
//
// I guess that the answer would be to make it an array of longs, bit shift each note 
// and or in the length of the note.  But that's some bullshit, so two arrays it is.
int melody[] = {
  NOTE_E4, NOTE_E4, REST, NOTE_E4, REST, NOTE_C4, NOTE_E4, REST,
  NOTE_G4, REST, NOTE_G3, REST,
  NOTE_C4, REST, NOTE_G3, REST, NOTE_E3, 
  REST, NOTE_A3, REST, NOTE_B3, REST, NOTE_AS3, NOTE_A3,
  NOTE_G3, NOTE_E4, NOTE_G4, NOTE_A4, NOTE_F4, NOTE_G4,
  REST, NOTE_E4, REST, NOTE_C4, NOTE_D4, NOTE_B3, REST,
  NOTE_C4, REST, NOTE_G3, REST, NOTE_E3, 
  REST, NOTE_A3, REST, NOTE_B3, REST, NOTE_AS3, NOTE_A3,
  NOTE_G3, NOTE_E4, NOTE_G4, NOTE_A4, NOTE_F4, NOTE_G4,
  REST, NOTE_E4, REST, NOTE_C4, NOTE_D4, NOTE_B3, REST,
  REST, NOTE_G4, NOTE_FS4, NOTE_F4, NOTE_DS4, REST, NOTE_E4,
  REST, NOTE_GS3, NOTE_A3, NOTE_C4, REST, NOTE_A3, NOTE_C4, NOTE_D4,
  REST, NOTE_G4, NOTE_FS4, NOTE_F4, NOTE_DS4, REST, NOTE_E4,
  REST, NOTE_G4, REST, NOTE_G4, NOTE_G4, REST
  REST, NOTE_G4, NOTE_FS4, NOTE_F4, NOTE_DS4, REST, NOTE_E4,
  REST, NOTE_GS3, NOTE_A3, NOTE_C4, REST, NOTE_A3, NOTE_C4, NOTE_D4,
  REST, NOTE_DS4, REST, NOTE_D4, REST, NOTE_C4, REST, REST
};
// Reasonable not durations are:
// 1 - Whole note
// 2 - Half note
// 4 - Quarter note
// 8 - Eighth note
// 16 - Sizteenth note
// Of course, this makes triplets handwaver
int noteDurations[] = {
  8, 8, 8, 8, 8, 8, 8, 8,
  4, 4, 4, 4,
  4, 8, 8, 4, 4,
  8, 8, 8, 8, 8, 8, 4,
  6, 6, 6, 4, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 4,
  4, 8, 8, 4, 4,
  8, 8, 8, 8, 8, 8, 4,
  6, 6, 6, 4, 8, 8,
  4, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  4, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 4, 4,
  4, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  4, 4, 8, 8, 4, 4, 4, 2
};
int currentNote = 0;
long lastNoteEndsAt = -1;
int melodyLoopsAt = 12;
int whole_note = 1497; // length of a whole note

void playMelody() {
  // Only change notes if we're done with this one.
  if(0 <= lastNoteEndsAt && currentTimestamp() < lastNoteEndsAt) {
    return;
  }
  melodyPlaying = true;

  // New note.  Turn off the speaker.
  digitalWrite(SPEAKER, LOW);

  // If it's at the end, start over.
  if(sizeof(melody)/sizeof(int) <= currentNote) {
    currentNote = melodyLoopsAt;
  }
  // Get note an duration/
  int note = melody[currentNote];
  int duration = whole_note / noteDurations[currentNote];
  lastNoteEndsAt = currentTimestamp() + duration;
  currentNote++;

  if(0 < note) {
    tone(SPEAKER, note, duration);
  }
}

 int allHoroscopes[][5] = 
{
  { NOTE_E4, NOTE_E4, NOTE_E4, NOTE_C4, NOTE_C4 }, 
  { NOTE_E4, NOTE_G4, NOTE_G3, NOTE_C4, NOTE_C4 }, 
  { NOTE_G3, NOTE_E3, NOTE_A3, NOTE_B3, NOTE_C4 },
  { NOTE_AS3, NOTE_A3, NOTE_G3, NOTE_E4, NOTE_C4 }, 
  { NOTE_G4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_C4 }, 
  { NOTE_E4, NOTE_C4, NOTE_D4, NOTE_B3, NOTE_C4 }, 
  { NOTE_C4, NOTE_G3, NOTE_E3, NOTE_A3, NOTE_C4 }, 
  { NOTE_B3, NOTE_AS3, NOTE_A3, NOTE_G3, NOTE_C4 }, 
  { NOTE_E4, NOTE_G4, NOTE_A4, NOTE_F4, NOTE_C4 }, 
  { NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_C4 }, 
  { NOTE_B3, NOTE_G4, NOTE_FS4, NOTE_F4, NOTE_C4 }, 
  { NOTE_DS4, NOTE_E4, NOTE_GS3, NOTE_A3, NOTE_C4 }, 
  { NOTE_C4, NOTE_A3, NOTE_C4, NOTE_D4, NOTE_C4 }, 
  { NOTE_G4, NOTE_FS4, NOTE_F4, NOTE_DS4, NOTE_C4 }, 
  { NOTE_E4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_C4 },
  { NOTE_G4, NOTE_FS4, NOTE_F4, NOTE_DS4, NOTE_C4 },
  { NOTE_E4, NOTE_GS3, NOTE_A3, NOTE_C4, NOTE_C4 },
  { NOTE_A3, NOTE_C4, NOTE_D4, NOTE_DS4, NOTE_C4 }
};

void setHoroscope() {
  int horo = random(18);
  horoscope[0] = allHoroscopes[horo][0];
  horoscope[1] = allHoroscopes[horo][1];
  horoscope[2] = allHoroscopes[horo][2];
  horoscope[3] = allHoroscopes[horo][3];
}



void playHoroscope() {
  if (0 <= lastNoteEndsAt && currentTimestamp() < lastNoteEndsAt) {
    return;  
  }

  digitalWrite(SPEAKER, LOW);

  if (sizeof(horoscope)/sizeof(int) <= currentNote ) {
    tone(SPEAKER, 0);
    noTone(SPEAKER);
    digitalWrite(SPEAKER, LOW);
    currentNote = 0;
    lastNoteEndsAt = -1;
    ceasedPlaying = 0;
    Serial.println("Finished Horoscope");
    return;
  }

  Serial.println("playing tone");


  int note = horoscope [currentNote];
  int duration = whole_note / random(12); 
  lastNoteEndsAt = currentTimestamp() + duration;
  currentNote++;

  if (0 < note){
    tone(SPEAKER, note, duration);  
  }
}

void ceasePlaying() {
  tone(SPEAKER, 0);
  noTone(SPEAKER);  
  digitalWrite(SPEAKER, LOW);
  melodyPlaying = false;
  currentNote = 0;
  lastNoteEndsAt = -1;
  ceasedPlaying = currentTimestamp();
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

// reading out the clock
void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

int getHourFromRTC(){
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);

  return int(hour);
}

int getMinuteFromRTC(){
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  
  return int(minute);
}

int getDayFromRTC(){
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);

  return int(dayOfMonth);
}

// display current time on Serial
void displayTime()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  // send it to the serial monitor
  Serial.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  Serial.print(":");
  if (minute<10)
  {
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second<10)
  {
    Serial.print("0");
  }
  Serial.print(second, DEC);
  Serial.print(" ");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print(" Day of week: ");
  switch(dayOfWeek){
  case 1:
    Serial.println("Sunday");
    break;
  case 2:
    Serial.println("Monday");
    break;
  case 3:
    Serial.println("Tuesday");
    break;
  case 4:
    Serial.println("Wednesday");
    break;
  case 5:
    Serial.println("Thursday");
    break;
  case 6:
    Serial.println("Friday");
    break;
  case 7:
    Serial.println("Saturday");
    break;
  }
}
