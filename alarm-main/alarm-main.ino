#include <Wire.h>
#include <RTClib.h>
#include <SevSeg.h>
#include <Encoder.h>
#include "pitches.h"
#include <SPI.h>
#include <RFID.h>

RTC_DS1307 rtc;
SevSeg sevseg; // Create an instance of the SevSeg object
Encoder encoder(32, 33); // Rotary encoder connected to pins 32 and 33
const int buttonPin = 34; // Button connected to pin 34
const int speakerPin = 29; // Speaker connected to pin 40

//RFID Sensor

int SDA_DIO = 48;
int RESET_DIO = 49;
RFID RC522(SDA_DIO, RESET_DIO); 


//


//Motor Stuff
const int motor1pin1 = 39;
const int motor1pin2 = 41;
const int motor2pin1 = 43;
const int motor2pin2 = 45;
const int motor1en = 44;
const int motor2en = 46;

bool turnedLeft = false; //flag for turning
long turnTime = 0; //clock for time when turn is initiated
long turnDuration = 2000; //time of turn in millis


//End of motor stuff

uint32_t counter = 0;
unsigned long previousMillis = 0;  // Stores the last time the loop was updated
const long interval = 1000;        // Interval at which to read sensors and update time (milliseconds)
unsigned long blinkMillis = 0;     // Stores the last time the display was toggled
const long blinkInterval = 400;    // Interval at which to blink the display (milliseconds)
unsigned long alarmMillis = 0;     // Stores the time when the alarm started
const long alarmDuration = 30000;  // Duration for which the alarm sound should play (milliseconds)
unsigned long lastButtonPress = 0; // Stores the last time the button was pressed
const long debounceDelay = 150;     // Debounce delay for the button (milliseconds)

int alarmHour = 0; // Alarm hour
int alarmMinute = 0; // Alarm minute
int buttonPressCount = 0; // Count the number of button presses
bool buttonPressed = false; // Flag to indicate if the button is pressed
bool settingAlarm = false; // Flag to indicate if the alarm is being set
bool displayOn = true; // Flag to indicate if the display is on or off for blinking
bool alarmTriggered = false; // Flag to indicate if the alarm has been triggered
bool alarmOff = true;

int melody[] = {
  NOTE_E5, NOTE_D5, NOTE_FS4, NOTE_GS4, 
  NOTE_CS5, NOTE_B4, NOTE_D4, NOTE_E4, 
  NOTE_B4, NOTE_A4, NOTE_CS4, NOTE_E4,
  NOTE_A4
};

int durations[] = {
  8, 8, 4, 4,
  8, 8, 4, 4,
  8, 8, 4, 4,
  2
};

struct Note {
    int frequency;
    int duration;
};

Note notes[] = {
    {207, 80}, {0, 83}, {207, 80}, {0, 83}, {349, 80}, {0, 83}, {349, 80}, {0, 83},
    {349, 80}, {0, 83}, {349, 80}, {0, 250}, {311, 80}, {0, 250}, {261, 80}, {0, 83},
    {311, 80}, {0, 83}, {311, 80}, {0, 83}, {311, 80}, {0, 83}, {311, 80}, {0, 250},
    {261, 80}, {0, 250}, {261, 80}, {0, 83}, {277, 80}, {0, 83}, {277, 80}, {0, 83},
    {277, 80}, {0, 83}, {277, 80}, {0, 250}, {261, 80}, {0, 250}, {233, 80}, {0, 83},
    {261, 80}, {87, 162}, {130, 80}, {0, 83}, {349, 80}, {0, 250}, {261, 80}, {0, 83},
    {207, 80}, {0, 83}, {207, 80}, {0, 83}, {349, 80}, {0, 83}, {349, 80}, {0, 83},
    {349, 80}, {0, 83}, {349, 80}, {0, 250}, {311, 80}, {0, 250}, {261, 80}, {0, 83},
    {311, 80}, {0, 83}, {311, 80}, {0, 83}, {311, 80}, {0, 83}, {311, 80}, {0, 250},
    {261, 80}, {0, 250}, {261, 80}, {0, 83}, {233, 80}, {0, 83}, {233, 80}, {0, 83},
    {233, 80}, {0, 83}, {233, 80}, {0, 250}, {261, 80}, {0, 250}, {233, 80}, {0, 83},
    {311, 727}, {155, 323}, {0, 666}, {174, 80}, {0, 250}, {174, 80}, {0, 250},
    {155, 80}, {0, 250}, {830, 80}, {0, 83}, {415, 80}, {0, 83}, {493, 80}, {0, 83},
    {415, 80}, {0, 83}, {311, 80}, {0, 83}, {311, 80}, {0, 83}, {523, 80}, {0, 83},
    {311, 80}, {0, 83}, {523, 80}, {0, 83}, {415, 80}, {0, 83}, {523, 80}, {0, 83},
    {311, 80}, {0, 250}, {415, 80}, {0, 250}, {311, 80}, {0, 83}, {466, 80}, {0, 83},
    {349, 80}, {0, 83}, {466, 80}, {0, 83}, {349, 80}, {0, 83}, {466, 80}, {0, 83},
    {466, 80}, {0, 250}, {349, 80}, {0, 83}, {466, 80}, {0, 83}, {466, 80}, {0, 83},
    {233, 80}, {0, 83}, {220, 80}, {0, 83}, {233, 80}, {0, 83}, {293, 80}, {0, 83},
    {311, 80}, {0, 83}, {466, 80}, {0, 83}, {391, 80}, {0, 83}, {311, 80}, {0, 83},
    {391, 80}, {0, 83}, {391, 80}, {0, 250}, {349, 80}, {0, 83}, {369, 80}, {0, 83},
    {783, 80}, {0, 83}, {391, 80}, {0, 83}, {932, 80}, {0, 83}, {391, 80}, {0, 83},
    {391, 80}, {0, 250}, {311, 80}, {0, 83}, {466, 80}, {0, 83}, {311, 80}, {0, 83},
    {415, 80}, {0, 83}, {311, 80}, {0, 83}, {415, 80}, {0, 83}, {311, 80}, {0, 250},
    {311, 80}, {0, 83}, {466, 80}, {0, 83}, {415, 80}, {0, 83}, {523, 80}, {0, 83},
    {415, 80}, {0, 83}, {103, 80}, {0, 250}, {97, 80}, {0, 250}, {92, 80}, {0, 250},
    {87, 80}, {0, 83}, {174, 80}, {0, 83}, {174, 80}, {0, 83}, {174, 80}, {0, 83},
    {174, 80}, {0, 83}, {174, 80}, {0, 83}, {174, 80}, {0, 83}, {174, 80}, {0, 83},
    {174, 80}, {0, 83}, {174, 80}, {0, 83}, {174, 80}, {0, 83}, {174, 80}, {0, 83},
    {174, 80}, {0, 83}, {174, 80}, {0, 83}, {174, 80}, {0, 83}, {233, 80}, {0, 83},
    {233, 80}, {0, 83}, {233, 80}, {0, 83}, {233, 80}, {0, 83}, {233, 80}, {0, 83},
    {233, 80}, {0, 83}, {233, 80}, {0, 83}, {233, 80}, {0, 83}, {233, 80}, {0, 250},
    {932, 80}, {880, 80}, {932, 80}, {0, 750}, {195, 80}, {0, 83}, {195, 80}, {0, 83},
    {195, 80}, {0, 83}, {195, 80}, {0, 83}, {195, 80}, {0, 83}, {195, 80}, {0, 83},
    {195, 80}, {0, 83}, {195, 80}, {0, 83}, {195, 80}, {0, 83}, {195, 80}, {0, 83},
    {195, 80}, {0, 83}, {195, 80}, {0, 83}, {195, 80}, {0, 83}, {195, 80}, {0, 83},
    {195, 80}, {0, 83}, {195, 80}, {0, 83}, {195, 80}, {0, 83}, {311, 80}, {0, 83},
    {311, 80}, {0, 83}, {311, 80}, {0, 83}, {311, 80}, {0, 83}, {311, 80}, {0, 83},
    {311, 80}, {0, 83}, {349, 80}, {0, 250}, {311, 80}, {0, 250}, {277, 80}, {0, 250},
    {261, 80}, {1046, 80}, {1046, 80}, {1046, 80}, {233, 80}, {0, 250}, {415, 80},
    {0, 83}, {415, 80}, {0, 83}, {493, 80}, {0, 83}, {415, 80}, {0, 83}, {311, 80},
    {0, 83}, {415, 80}, {0, 83}, {523, 80}, {0, 83}, {415, 80}, {0, 83}, {523, 80},
    {0, 83}, {415, 80}, {0, 83}, {523, 80}, {0, 83}, {415, 80}, {0, 250}, {415, 80},
    {0, 83}, {523, 80}, {0, 83}, {415, 80}, {0, 83}, {554, 80}, {0, 83}, {554, 80},
    {0, 83}, {554, 80}, {0, 83}, {554, 80}, {0, 83}, {554, 80}, {0, 83}, {554, 80},
    {0, 250}, {415, 80}, {0, 250}, {554, 80}, {0, 83}, {554, 80}, {0, 83}, {554, 80},
    {0, 83}, {261, 80}, {0, 250}, {233, 80}, {0, 250}, {207, 80}, {0, 83}, {523, 80},
    {0, 83}, {415, 80}, {0, 83}, {415, 80}, {0, 83}, {369, 80}, {0, 250}, {369, 80},
    {0, 250}, {369, 80}, {0, 83}, {739, 80}, {0, 83}, {369, 80}, {0, 83}, {369, 80},
    {0, 83}, {349, 80}, {0, 250}, {349, 80}, {0, 250}, {233, 80}, {0, 250}, {233, 80},
    {0, 83}, {233, 80}, {0, 83}, {311, 80}, {0, 83}, {622, 80}, {0, 83}, {311, 80},
    {0, 83}, {311, 80}, {0, 83}, {349, 80}, {0, 83}, {523, 80}, {0, 83}, {466, 80},
    {0, 83}, {349, 80}, {0, 83}, {523, 80}, {0, 83}, {349, 80}, {0, 250}, {349, 80},
    {0, 83}, {349, 80}, {0, 83}, {311, 80}, {523, 80}, {0, 83}, {415, 80}, {0, 83},
    {523, 80}, {0, 83}, {311, 80}, {0, 250}, {311, 80}, {0, 250}, {277, 80}, {0, 83},
    {739, 80}, {0, 83}, {277, 80}, {0, 83}, {739, 80}, {0, 83}, {261, 80}, {0, 250},
    {261, 80}, {0, 250}, {174, 80}, {0, 83}, {466, 80}, {0, 83}, {233, 80}, {0, 250},
    {311, 80}, {0, 83}, {622, 80}, {0, 83}, {233, 80}, {0, 250}, {103, 80}, {0, 83},
    {103, 80}, {0, 83}, {103, 80}, {0, 83}, {103, 80}, {207, 80}, {103, 80}, {0, 666},
    {1174, 80}, {1244, 80}, {0, 250}, {932, 404}, {0, 333}, {1108, 80}, {1174, 80},
    {0, 83}, {1174, 80}, {0, 83}, {987, 80}, {0, 83}, {987, 80}, {0, 167}, {1046, 80},
    {0, 83}, {1046, 80}, {987, 80}, {987, 80}, {0, 83}, {987, 80}, {0, 83}, {1046, 80},
    {0, 83}, {880, 80}, {0, 83}, {880, 80}, {0, 750}, {207, 80}
};

unsigned long midiPreviousMillis = 0;
int noteIndex = 0;
bool isPlaying = false;

DateTime now;

void setup() {
  
  Serial.begin(115200);
  Serial.println();
  Serial.println(__FILE__);

    /* Enable the SPI interface */
  SPI.begin(); 
  /* Initialise the RFID reader */
  RC522.init();

  // Initialize I2C communication
  Wire.begin();
  Serial.println("I2C bus initialized.");

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  Serial.println("RTC initialized.");

  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // Following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("RTC time set.");
  }

  alarmHour = rtc.now().hour();
  // alarmMinute = rtc.now().minute() + 0;
  // alarmMinute = rtc.now().minute() + 1;
  alarmMinute = rtc.now().minute() + 5;

  // Initialize the 7-segment display
  byte numDigits = 4;
  byte digitPins[] = {22, 23, 24, 25};
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  bool updateWithDelays = true; // Default 'false' is recommended
  bool leadingZeros = true; // Use 'true' if you want leading zeros

  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros);
  sevseg.setBrightness(50); // Adjust brightness as needed

  // Initialize button pin
  pinMode(buttonPin, INPUT_PULLUP);
  // Initialize speaker pin
  pinMode(speakerPin, OUTPUT);

  //Initialize motors
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1,   OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  pinMode(motor1en,   OUTPUT); 
  pinMode(motor2en, OUTPUT);
  stopMotors();

  analogWrite(motor1en, 150); //ENA   pin
  analogWrite(motor2en, 150); //ENB pin

  // Test the speaker
  tone(speakerPin, 1000); // Play a 1000 Hz tone on the speaker
  delay(1000); // Wait for 1 second
  noTone(speakerPin); // Stop the sound
}


void loop() {
  sevseg.refreshDisplay(); // Must be called repeatedly
  unsigned long currentMillis = millis();
  
  // Check if the button is pressed
  if (digitalRead(buttonPin) == LOW && (currentMillis - lastButtonPress > debounceDelay)) {
    lastButtonPress = currentMillis;
    buttonPressCount++;
    if (buttonPressCount > 2) {
      // Confirm the alarm set time and switch back to clock mode
      settingAlarm = false;
      buttonPressCount = 0;
    } else {
      settingAlarm = true; // Start setting the alarm if the encoder is moved
    }

    // Stop the alarm if it is currently triggered
    if (!alarmOff) {
      //noTone(speakerPin); // Stop the sound
      alarmOff = true; // Reset the alarm triggered flag
    }
  }
  

  if (RC522.isCard()) {
      alarmOff = true; // Reset the alarm triggered flag
  }

  // Blink the display if setting the alarm
  if (settingAlarm) {
    if (currentMillis - blinkMillis >= blinkInterval) {
      blinkMillis = currentMillis;
      displayOn = !displayOn; // Toggle display state
    }
    if (displayOn) {
      int displayAlarmTime = (alarmHour * 100) + alarmMinute; // Format HHMM
      sevseg.setNumber(displayAlarmTime, 2); // 2 decimal places for HH:MM
    } else {
      sevseg.blank(); // Turn off display
    }
  } else {
    // Display the current time on the 7-segment display
    DateTime now = rtc.now();
    int displayCurrentTime = (now.hour() * 100) + now.minute(); // Format HHMM
    sevseg.setNumber(displayCurrentTime, 2); // 2 decimal places for HH:MM
  }

  // Read the rotary encoder
  long newPosition = encoder.read() / 4; // Adjust for encoder resolution
  if (newPosition != 0) {
    if (buttonPressCount == 1) {
      alarmHour = (alarmHour + newPosition) % 24;
      if (alarmHour < 0) alarmHour += 24;
    } else if (buttonPressCount == 2) {
      alarmMinute = (alarmMinute + newPosition) % 60;
      if (alarmMinute < 0) alarmMinute += 60;
    }
    encoder.write(0); // Reset encoder position
  }

  // Update RTC time every second
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (counter % 10 == 0) {
      Serial.println("\nCNT");
    }

    Serial.print(counter);
    Serial.println();

    counter++;

    // Read and print RTC time
    now = rtc.now();
    Serial.print("Current Date and Time: ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.println(now.second(), DEC);

    // Check if the current time matches the alarm time
    if (now.hour() == alarmHour && now.minute() == alarmMinute) {
      // Trigger alarm action (e.g., sound a buzzer, turn on an LED, etc.)
      Serial.print("ALARM! Time to wake up!");
      //midi(); // Play the Nokia ringtone
      alarmMillis = currentMillis; // Record the time when the alarm started
      if (!alarmTriggered) alarmOff = false;
      alarmTriggered = true; // Set the alarm triggered flag
    }
  }


  //WITHOUT THE INTERVAL
  if (!alarmOff) {
    midi();
    if (random(10000) < 10) { //every once in a while, turn left
    turnLeft();
    turnedLeft = true;
    turnDuration = 500 + random(1000);
    Serial.println("Turning lefty");
    turnTime = currentMillis;
    } else if (!turnedLeft) {
      driveMotors();
    }
  } else {
    stopMotors();
  }

  if (turnedLeft and currentMillis - turnTime > turnDuration) {
    turnedLeft = false;
  }

  // Stop the alarm sound after the specified duration
  if (!alarmOff && currentMillis - alarmMillis >= alarmDuration) {
    Serial.println("Turning off alarm");
    noTone(speakerPin); // Stop the sound
    alarmTriggered = false; // Reset the alarm triggered flag
    alarmOff = true;
  }
}

void midi() {
    unsigned long midiCurrentMillis = millis();
    if (noteIndex < sizeof(notes) / sizeof(notes[0])) {
        if (!isPlaying) {
            tone(speakerPin, notes[noteIndex].frequency, notes[noteIndex].duration);
            midiPreviousMillis = midiCurrentMillis;
            isPlaying = true;
        } else {
            if (midiCurrentMillis - midiPreviousMillis >= notes[noteIndex].duration) {
                // Serial.println("HEEEYYYYYYYYY");
                noTone(speakerPin);
                midiPreviousMillis = midiCurrentMillis;
                noteIndex++;
                isPlaying = false;
            }
        }
    } else {
      noteIndex = 0;
    }
}

void driveMotors() {
  // Serial.println("Driving");
  digitalWrite(motor1pin1,   LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
}

void turnLeft() {
  Serial.println("Turning Left");
  digitalWrite(motor1pin1,   LOW);
  digitalWrite(motor1pin2, LOW);

  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
}

void stopMotors() {
  // Serial.println("Stopping");
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
}
